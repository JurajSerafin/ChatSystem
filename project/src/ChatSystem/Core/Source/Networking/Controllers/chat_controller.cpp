#include "Networking/Controllers/chat_controller.h"
#include "Networking/api_errors.h"
#include "Chat/chat.h"

#include <nlohmann/json.hpp>
#include <boost/url.hpp>

namespace {
  constexpr std::string_view kJsonStrMime = "application/json";
  constexpr std::string_view kAuthSchemeIdf = "Bearer ";
  constexpr std::size_t kAuthSchemeIdfPrefixLen = 7;

  constexpr std::string_view kParticipantIdsField = "participant_ids";

  // JSON Keys (Must be const char* for nlohmann::json initializer lists)
  constexpr const char* kIdField = "id";
  constexpr const char* kCreatedAtField = "created_at";
  constexpr const char* kUserIdField = "user_id";
  constexpr const char* kUsernameField = "login";
  constexpr const char* kTagField = "tag";

  // Path Param Keys
  constexpr std::string_view kPathParamChatId = "id";
  constexpr std::string_view kPathParamUserId = "user_id";
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

    return BuildAndReturnOkResponse(req, FormatJsonOutput(new_chat));
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

    ExtractPaginationLimitAndOffset(req, limit, offset);

    const auto chats = chat_service_obs_->GetChatsForUser(*caller_id, limit, offset);
    return BuildAndReturnOkResponse(req, FormatJsonOutput(chats));
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

    const ChatId chat_id = ChatId::Reconstitute(ExtractPathParam(params, kPathParamChatId));

    const auto chat = chat_service_obs_->GetChatById(chat_id, *caller_id);

    if (!chat) {
      return api::errors::NotFound(req, "Chat not found.");
    }

    return BuildAndReturnOkResponse(req, FormatJsonOutput(*chat));
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

    const ChatId chat_id = ChatId::Reconstitute(ExtractPathParam(params, kPathParamChatId));

    const auto participants = chat_service_obs_->GetParticipants(chat_id, *caller_id);

    return BuildAndReturnOkResponse(req, FormatJsonOutput(participants));
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

    const ChatId chat_id = ChatId::Reconstitute(ExtractPathParam(params, kPathParamChatId));

    auto body = nlohmann::json::parse(req.body());

    const UserId new_user_id = UserId::Reconstitute(body.at(kUserIdField).get<std::string>());

    chat_service_obs_->AddParticipant(chat_id, *caller_id, new_user_id);

    return BuildAndReturnNoContentResponse(req);
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

    const ChatId chat_id = ChatId::Reconstitute(ExtractPathParam(params, kPathParamChatId));

    const UserId user_to_remove = UserId::Reconstitute(ExtractPathParam(params, kPathParamUserId));

    chat_service_obs_->RemoveParticipant(chat_id, *caller_id, user_to_remove);

    return BuildAndReturnNoContentResponse(req);
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

    const ChatId chat_id = ChatId::Reconstitute(ExtractPathParam(params, kPathParamChatId));

    chat_service_obs_->DeleteChat(chat_id, *caller_id);

    return BuildAndReturnNoContentResponse(req);
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

std::optional<UserId> ChatController::GetAuthenticatedUserId(const http::request<http::string_body>& req) const {
  const std::string auth_header = GetAuthHeader(req);

  if (!IsValidToken(auth_header)) {
    return std::nullopt;
  }

  return auth_service_obs_->ValidateToken(GetToken(auth_header))->GetId();
}

std::string ChatController::ExtractPathParam(const Router::PathParams& params, std::string_view key) {
  for (auto&& [param_key, param_val] : params) {
    if (param_key == key) {
      return std::string(param_val);
    }
  }
  throw std::invalid_argument("Missing required path parameter.");
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


http::response<http::string_body> ChatController::BuildAndReturnOkResponse(
  const http::request<http::string_body>& req,
  const nlohmann::json& responseJson)
{
  http::response<http::string_body> res{ http::status::ok, req.version() };

  res.set(http::field::content_type, kJsonStrMime);

  res.body() = responseJson.dump();

  res.prepare_payload();

  return res;
}

http::response<http::string_body> ChatController::BuildAndReturnNoContentResponse(
  const http::request<http::string_body>& req)
{
  http::response<http::string_body> res{ http::status::no_content, req.version() };

  res.prepare_payload();

  return res;
}

bool ChatController::IsValidToken(std::string_view header) {
  return header.starts_with(kAuthSchemeIdf);
}

std::string ChatController::GetToken(const std::string& header) {
  return header.substr(kAuthSchemeIdfPrefixLen);
}

std::string ChatController::GetAuthHeader(const http::request<http::string_body>& req) {
  return std::string(req[http::field::authorization]);
}

void ChatController::ExtractPaginationLimitAndOffset(
  const http::request<http::string_body>& req,
  std::size_t& limitOut,
  std::size_t& offsetOut)
{
  if (auto rv = urls::parse_origin_form(req.target())) {
    const auto params = rv->params();

    if (const auto it = params.find("limit"); it != params.end()) {
      limitOut = std::stoull(it->value);
    }
    if (const auto it = params.find("offset"); it != params.end()) {
      offsetOut = std::stoull(it->value);
    }
  }
}