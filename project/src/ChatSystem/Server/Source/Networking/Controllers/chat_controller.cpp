#include "Networking/Controllers/chat_controller.h"

#include "Chat/chat.h"
#include "Networking/Controllers/utils.h"
#include "Networking/api_errors.h"
#include "Api/chat_contract.h"

#include <nlohmann/json.hpp>

namespace {
  constexpr std::size_t kDefaultChatsPaginationLimit = 50;
} // namespace

// POST /chats
http::response<http::string_body> ChatController::HandleCreateChat(
  const http::request<http::string_body>& req,
  const Router::PathParams&) const
{
  try {
    const auto caller_id = GetAuthenticatedUserId(req);

    if (!caller_id) {
      return api::errors::Unauthorized(req);
    }

    const auto participant_ids = ExtractParticipantIds(nlohmann::json::parse(req.body()));

    const auto new_chat = chat_service_obs_->CreateChat(*caller_id, participant_ids);

    return netw::utils::BuildAndReturnOkResponse(req, FormatJsonOutput(new_chat));
  }
  catch (const nlohmann::json::exception&) {
    return api::errors::BadRequest(req, "Invalid JSON format.");
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

// GET /chats?limit=&offset=
http::response<http::string_body> ChatController::HandleGetChats(
  const http::request<http::string_body>& req,
  const Router::PathParams&) const
{
  try {
    const auto caller_id = GetAuthenticatedUserId(req);

    if (!caller_id) {
      return api::errors::Unauthorized(req);
    }

    std::size_t limit = kDefaultChatsPaginationLimit;
    std::size_t offset = 0;

    netw::utils::ExtractPaginationLimitAndOffset(req, limit, offset);

    const auto chats = chat_service_obs_->GetChatsForUser(*caller_id, limit, offset);

    return netw::utils::BuildAndReturnOkResponse(req, FormatJsonOutput(chats));
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

// GET /chats/{id}
http::response<http::string_body> ChatController::HandleGetChatById(
  const http::request<http::string_body>& req,
  const Router::PathParams& params) const
{
  try {
    const auto caller_id = GetAuthenticatedUserId(req);

    if (!caller_id) {
      return api::errors::Unauthorized(req);
    }

    const ChatId chat_id = ChatId::Reconstitute(netw::utils::ExtractPathParam(params, api::chat::path_params::kChatId));

    const auto chat = chat_service_obs_->GetById(chat_id, *caller_id);

    if (!chat) {
      return api::errors::NotFound(req, "Chat not found.");
    }

    return netw::utils::BuildAndReturnOkResponse(req, FormatJsonOutput(*chat));
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

// GET /chats/{id}/participants
http::response<http::string_body> ChatController::HandleGetParticipants(
  const http::request<http::string_body>& req,
  const Router::PathParams& params) const
{
  try {
    const auto caller_id = GetAuthenticatedUserId(req);

    if (!caller_id) {
      return api::errors::Unauthorized(req);
    }

    const ChatId chat_id = ChatId::Reconstitute(netw::utils::ExtractPathParam(params, api::chat::path_params::kChatId));

    const auto participants = chat_service_obs_->GetParticipants(chat_id, *caller_id);

    return netw::utils::BuildAndReturnOkResponse(req, FormatJsonOutput(participants));
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

// POST /chats/{id}/participants
http::response<http::string_body> ChatController::HandleAddParticipant(
  const http::request<http::string_body>& req,
  const Router::PathParams& params) const
{
  try {
    const auto caller_id = GetAuthenticatedUserId(req);

    if (!caller_id) {
      return api::errors::Unauthorized(req);
    }

    const ChatId chat_id = ChatId::Reconstitute(netw::utils::ExtractPathParam(params, api::chat::path_params::kChatId));

    auto body = nlohmann::json::parse(req.body());

    const UserId new_user_id = UserId::Reconstitute(body.at(api::chat::fields::kUserId).get<std::string>());

    chat_service_obs_->AddParticipant(chat_id, *caller_id, new_user_id);

    return netw::utils::BuildAndReturnNoContentResponse(req);
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

// DELETE /chats/{id}/participants/{user_id}
http::response<http::string_body> ChatController::HandleRemoveParticipant(
  const http::request<http::string_body>& req,
  const Router::PathParams& params) const
{
  try {
    const auto caller_id = GetAuthenticatedUserId(req);

    if (!caller_id) {
      return api::errors::Unauthorized(req);
    }

    const ChatId chat_id = ChatId::Reconstitute(netw::utils::ExtractPathParam(params, api::chat::path_params::kChatId));

    const UserId user_to_remove = UserId::Reconstitute(netw::utils::ExtractPathParam(params, api::chat::path_params::kUserId));

    chat_service_obs_->RemoveParticipant(chat_id, *caller_id, user_to_remove);

    return netw::utils::BuildAndReturnNoContentResponse(req);
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

// DELETE /chats/{id}
http::response<http::string_body> ChatController::HandleDeleteChat(
  const http::request<http::string_body>& req,
  const Router::PathParams& params) const
{
  try {
    const auto caller_id = GetAuthenticatedUserId(req);

    if (!caller_id) {
      return api::errors::Unauthorized(req);
    }

    const ChatId chat_id = ChatId::Reconstitute(netw::utils::ExtractPathParam(params, api::chat::path_params::kChatId));

    chat_service_obs_->DeleteChat(chat_id, *caller_id);

    return netw::utils::BuildAndReturnNoContentResponse(req);
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

std::optional<UserId> ChatController::GetAuthenticatedUserId(const http::request<http::string_body>& req) const {
  const std::string auth_header = netw::utils::GetAuthHeader(req);

  if (!netw::utils::IsValidToken(auth_header)) {
    return std::nullopt;
  }

  return auth_service_obs_->ValidateToken(netw::utils::GetToken(auth_header))->GetId();
}

std::vector<UserId> ChatController::ExtractParticipantIds(const nlohmann::json& reqBody) {
  std::vector<UserId> participant_ids;

  for (auto&& id_str : reqBody.at(api::chat::fields::kParticipantIds)) {
    participant_ids.push_back(UserId::Reconstitute(id_str.get<std::string>()));
  }

  return participant_ids;
}

nlohmann::json ChatController::FormatJsonOutput(const Chat& chat) {
  nlohmann::json j = {
      {api::chat::fields::kId, chat.GetId().ToString()},
      {api::chat::fields::kCreatedAt, std::chrono::duration_cast<std::chrono::seconds>(chat.CreatedAt().time_since_epoch()).count()},
      {api::chat::fields::kLastActivityAt, std::chrono::duration_cast<std::chrono::seconds>(chat.CreatedAt().time_since_epoch()).count()},
      {api::chat::fields::kName, nullptr}
  };

  if (chat.GetLastMessage().has_value()) {
    j[api::chat::fields::kLastMessageId] = chat.GetLastMessage()->GetId().ToString();
  }
  else {
    j[api::chat::fields::kLastMessageId] = nullptr;
  }

  return j;
}

nlohmann::json ChatController::FormatJsonOutput(const std::vector<Chat>& chats) {
  auto json_array = nlohmann::json::array();

  for (const auto& chat : chats) {
    json_array.push_back(FormatJsonOutput(chat));
  }
  return json_array;
}


void ChatController::RegisterRoutes(Router& router) {
  router.AddRoute(http::verb::post, std::string{ api::chat::routes::kBase },
    [this](const auto& req, const auto& params) { return HandleCreateChat(req, params); }
  );

  router.AddRoute(http::verb::get, std::string{ api::chat::routes::kBase },
    [this](const auto& req, const auto& params) { return HandleGetChats(req, params); }
  );

  router.AddRoute(http::verb::get, std::string{ api::chat::routes::kById },
    [this](const auto& req, const auto& params) { return HandleGetChatById(req, params); }
  );

  router.AddRoute(http::verb::get, std::string{ api::chat::routes::kParticipants },
    [this](const auto& req, const auto& params) { return HandleGetParticipants(req, params); }
  );

  router.AddRoute(http::verb::post, std::string{ api::chat::routes::kParticipants },
    [this](const auto& req, const auto& params) {return HandleAddParticipant(req, params); }
  );

  router.AddRoute(http::verb::delete_, std::string{ api::chat::routes::kDeleteParticipant },
    [this](const auto& req, const auto& params) { return HandleRemoveParticipant(req, params); }
  );

  router.AddRoute(http::verb::delete_, std::string{ api::chat::routes::kById },
    [this](const auto& req, const auto& params) { return HandleDeleteChat(req, params); }
  );
}