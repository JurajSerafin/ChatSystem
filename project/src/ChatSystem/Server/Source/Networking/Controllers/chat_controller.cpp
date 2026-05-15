#include "Networking/Controllers/chat_controller.h"

#include "Chat/chat.h"
#include "Networking/Controllers/utils.h"
#include "Networking/api_errors.h"

#include <nlohmann/json.hpp>

namespace {
  constexpr std::string_view kParticipantIdsField = "participant_ids";

  constexpr std::string_view kIdField = "id";
  constexpr std::string_view kCreatedAtField = "created_at";
  constexpr std::string_view kUserIdField = "user_id";
  constexpr std::string_view kUsernameField = "login";
  constexpr std::string_view kTagField = "tag";

  constexpr std::string_view kPathParamChatId = "id";
  constexpr std::string_view kPathParamUserId = "user_id";


  constexpr std::string_view kChatsRoute = "/chats";
  constexpr std::string_view kGetChatByIdRoute = "/chats/{id}";
  constexpr std::string_view kParticipantsRoute = "/chats/{id}/participants";
  constexpr std::string_view kDeleteParticipantRoute = "/chats/{id}/participants/{user_id}";
}  // namespace


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

    std::size_t limit = 50;

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

    const ChatId chat_id = ChatId::Reconstitute(netw::utils::ExtractPathParam(params, kPathParamChatId));

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

    const ChatId chat_id = ChatId::Reconstitute(netw::utils::ExtractPathParam(params, kPathParamChatId));

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

    const ChatId chat_id = ChatId::Reconstitute(netw::utils::ExtractPathParam(params, kPathParamChatId));

    auto body = nlohmann::json::parse(req.body());

    const UserId new_user_id = UserId::Reconstitute(body.at(kUserIdField).get<std::string>());

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

    const ChatId chat_id = ChatId::Reconstitute(netw::utils::ExtractPathParam(params, kPathParamChatId));

    const UserId user_to_remove = UserId::Reconstitute(netw::utils::ExtractPathParam(params, kPathParamUserId));

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

    const ChatId chat_id = ChatId::Reconstitute(netw::utils::ExtractPathParam(params, kPathParamChatId));

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

  for (auto&& id_str : reqBody.at(kParticipantIdsField)) {
    participant_ids.push_back(UserId::Reconstitute(id_str.get<std::string>()));
  }

  return participant_ids;
}

nlohmann::json ChatController::FormatJsonOutput(const Chat& chat) {
  return {
      {kIdField, chat.GetId().ToString()},
      {kCreatedAtField, std::chrono::duration_cast<std::chrono::seconds>(chat.CreatedAt().time_since_epoch()).count()}
  };
}

nlohmann::json ChatController::FormatJsonOutput(const std::vector<Chat>& chats) {
  auto json_array = nlohmann::json::array();

  for (const auto& chat : chats) {
    json_array.push_back(FormatJsonOutput(chat));
  }
  return json_array;
}

nlohmann::json ChatController::FormatJsonOutput(const std::vector<User>& participants) {
  auto json_array = nlohmann::json::array();

  for (const auto& user : participants) {
    json_array.push_back({
      {kIdField, user.GetId().ToString()},
      {kUsernameField, user.GetLogin()},
      {kTagField, user.GetTag().ToString()}
      });
  }

  return json_array;
}

void ChatController::RegisterRoutes(Router& router) {
  router.AddRoute(http::verb::post, std::string{kChatsRoute},
    [this] (const auto& req, const auto& params) { return HandleCreateChat(req, params); }
  );

  router.AddRoute(http::verb::get, std::string{ kChatsRoute },
    [this] (const auto& req, const auto& params) { return HandleGetChats(req, params); }
  );

  router.AddRoute(http::verb::get, std::string{ kGetChatByIdRoute },
    [this](const auto& req, const auto& params) { return HandleGetChatById(req, params); }
  );

  router.AddRoute(http::verb::get, std::string{ kParticipantsRoute },
    [this] (const auto& req, const auto& params) { return HandleGetParticipants(req, params); }
  );

  router.AddRoute(http::verb::post, std::string{ kParticipantsRoute },
    [this](const auto& req, const auto& params) {return HandleAddParticipant(req, params); }
  );

  router.AddRoute(http::verb::delete_, std::string{ kDeleteParticipantRoute },
    [this](const auto& req, const auto& params) { return HandleRemoveParticipant(req, params); }
  );

  router.AddRoute(http::verb::delete_, std::string{ kChatsRoute },
    [this](const auto& req, const auto& params) { return HandleDeleteChat(req, params); }
  );
}
