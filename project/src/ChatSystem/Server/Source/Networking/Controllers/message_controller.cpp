#include "Networking/Controllers/message_controller.h"

#include "Networking/Controllers/utils.h"
#include "Networking/api_errors.h"
#include "TypeLibHelpers/domain_class_type_variant_factory.h"
#include "boost/url/parse.hpp"

#include <chrono>

namespace {
  // Path Param Keys
  constexpr std::string_view kPathParamChatId = "id";
  constexpr std::string_view kPathParamMsgId = "id";

  // JSON Body Keys
  constexpr std::string_view kCiphertextField = "ciphertext";
  constexpr std::string_view kEncryptedKeysField = "encrypted_keys";
  constexpr std::string_view kTypeField = "type";

  // JSON Response Keys
  constexpr std::string_view kIdField = "id";
  constexpr std::string_view kSenderIdField = "sender_id";
  constexpr std::string_view kChatIdField = "chat_id";
  constexpr std::string_view kCreatedAtField = "created_at";
  constexpr std::string_view kEncryptedKeyField = "encrypted_key";

  constexpr std::size_t kDefaultMsgHistoryPaginationLimit = 50;
  constexpr std::size_t kDefaultUndeliveredMsgPaginationLimit = 50;

  constexpr std::string_view kChatMessagesRoute = "/chats/{id}/messages";
  constexpr std::string_view kMessageKeyRoute = "/messages/{id}/key";
  constexpr std::string_view kReadMessageRoute = "/messages/{id}/read";
  constexpr std::string_view kGetUndeliveredRoute = "/messages/undelivered";
  constexpr std::string_view kMessageIdRoute = "/messages/{id}";
} // namespace

// POST /chats/{id}/messages
http::response<http::string_body> MessageController::HandleSendMessage(
  const http::request<http::string_body>& req,
  const Router::PathParams& params) const
{
  try {
    const auto caller_id = GetAuthenticatedUserId(req);

    if (!caller_id) {
      return api::errors::Unauthorized(req);
    }

    ChatId chat_id = ChatId::Reconstitute(netw::utils::ExtractPathParam(params, kPathParamChatId));

    auto body = nlohmann::json::parse(req.body());

    auto ciphertext = body.at(kCiphertextField).get<std::string>();

    auto msg_type = DomainClassTypeVariantFactory<MessageTypeVariant>::Create(
      body.at(kTypeField).get<std::string>()
    );

    auto encrypted_keys = ExtractEncryptedKeys(body);

    const auto new_message = message_service_obs_->SendChatMessage(
      *caller_id, chat_id, ciphertext, encrypted_keys, msg_type
    );

    return netw::utils::BuildAndReturnOkResponse(req, FormatJsonOutput(new_message));
  }
  catch (const nlohmann::json::exception&) {
    return api::errors::BadRequest(req, "Invalid JSON format.");
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

// GET /chats/{id}/messages?limit=&offset=
http::response<http::string_body> MessageController::HandleGetMessageHistory(
  const http::request<http::string_body>& req,
  const Router::PathParams& params) const
{
  try {
    const auto caller_id = GetAuthenticatedUserId(req);

    if (!caller_id) {
      return api::errors::Unauthorized(req);
    }

    const ChatId chat_id = ChatId::Reconstitute(netw::utils::ExtractPathParam(params, kPathParamChatId));

    std::size_t limit = kDefaultMsgHistoryPaginationLimit;

    std::size_t offset = 0;

    netw::utils::ExtractPaginationLimitAndOffset(req, limit, offset);

    const auto messages = message_service_obs_->GetHistory(chat_id, *caller_id, limit, offset);

    return netw::utils::BuildAndReturnOkResponse(req, FormatJsonOutput(messages));
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

// GET /messages/{id}/key
http::response<http::string_body> MessageController::HandleGetMessagePublicKey(
  const http::request<http::string_body>& req,
  const Router::PathParams& params) const
{
  try {
    const auto caller_id = GetAuthenticatedUserId(req);

    if (!caller_id) {
      return api::errors::Unauthorized(req);
    }

    const MessageId msg_id = MessageId::Reconstitute(netw::utils::ExtractPathParam(params, kPathParamMsgId));

    auto encrypted_key = message_service_obs_->GetEncryptedKey(msg_id, *caller_id);

    if (!encrypted_key) {
      return api::errors::NotFound(req, "Encrypted key not found or you lack permission to access it.");
    }

    const nlohmann::json res_json = { {kEncryptedKeyField, *encrypted_key} };

    return netw::utils::BuildAndReturnOkResponse(req, res_json);
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

// PUT /messages/{id}/read
http::response<http::string_body> MessageController::HandleReadMessage(
  const http::request<http::string_body>& req,
  const Router::PathParams& params) const
{
  try {
    const auto caller_id = GetAuthenticatedUserId(req);

    if (!caller_id) {
      return api::errors::Unauthorized(req);
    }

    const MessageId msg_id = MessageId::Reconstitute(netw::utils::ExtractPathParam(params, kPathParamMsgId));

    message_service_obs_->MarkAsRead(msg_id, *caller_id);

    return netw::utils::BuildAndReturnNoContentResponse(req);
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

// GET /messages/undelivered?limit=&afterId=
http::response<http::string_body> MessageController::HandleGetUndelivered(
  const http::request<http::string_body>& req,
  const Router::PathParams&) const
{
  try {
    const auto caller_id = GetAuthenticatedUserId(req);

    if (!caller_id) {
      return api::errors::Unauthorized(req);
    }

    std::size_t limit = kDefaultUndeliveredMsgPaginationLimit;;

    std::optional<MessageId> after_id = std::nullopt;

    ExtractCursorPagination(req, limit, after_id);

    const auto messages = message_service_obs_->GetUndelivered(*caller_id, limit, after_id);

    return netw::utils::BuildAndReturnOkResponse(req, FormatJsonOutput(messages));
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

// DELETE /messages/{id}
http::response<http::string_body> MessageController::HandleDeleteMessage(
  const http::request<http::string_body>& req,
  const Router::PathParams& params) const
{
  try {
    const auto caller_id = GetAuthenticatedUserId(req);

    if (!caller_id) {
      return api::errors::Unauthorized(req);
    }

    const MessageId msg_id = MessageId::Reconstitute(netw::utils::ExtractPathParam(params, kPathParamMsgId));

    message_service_obs_->DeleteMessage(msg_id, *caller_id);

    return netw::utils::BuildAndReturnNoContentResponse(req);
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

std::optional<UserId> MessageController::GetAuthenticatedUserId(const http::request<http::string_body>& req) const {
  const auto token = netw::utils::ExtractToken(req);

  if (!token) {
    return std::nullopt;
  }

  return auth_service_obs_->ValidateToken(*token)->GetId();
}

nlohmann::json MessageController::FormatJsonOutput(const Message& msg) {
  return {
      {kIdField, msg.GetId().ToString()},
      {kSenderIdField, msg.GetSenderId().ToString()},
      {kChatIdField, msg.GetChatId().ToString()},
      {kCiphertextField, msg.GetCiphertext()},
      {kTypeField, msg.GetTypeStr()},
      {kCreatedAtField, std::chrono::duration_cast<std::chrono::seconds>(msg.CreatedAt().time_since_epoch()).count()}
  };
}

nlohmann::json MessageController::FormatJsonOutput(const std::vector<Message>& messages) {
  auto json_array = nlohmann::json::array();

  for (const auto& msg : messages) {
    json_array.push_back(FormatJsonOutput(msg));
  }

  return json_array;
}

EncryptedKeysMap MessageController::ExtractEncryptedKeys(const nlohmann::json& reqBody) {
  EncryptedKeysMap encrypted_keys;

  for (const auto& [user_id_str, key_val] : reqBody.at(kEncryptedKeysField).items()) {
    encrypted_keys[UserId::Reconstitute(user_id_str)] = key_val.get<std::string>();
  }

  return encrypted_keys;
}

void MessageController::ExtractCursorPagination(
  const http::request<http::string_body>& req,
  std::size_t& limitOut,
  std::optional<MessageId>& afterIdOut)
{
  if (auto rv = boost::urls::parse_origin_form(req.target())) {
    const auto params = rv->params();

    if (const auto it = params.find("limit"); it != params.end()) {
      limitOut = std::stoull(std::string((*it).value));
    }

    if (const auto it = params.find("afterId"); it != params.end()) {
      afterIdOut = MessageId::Reconstitute(std::string((*it).value));
    }
  }
}


void MessageController::RegisterRoutes(Router& router) {
  router.AddRoute(http::verb::post, std::string{ kChatMessagesRoute },
    [this](const auto& req, const auto& params) { return HandleSendMessage(req, params); }
  );

  router.AddRoute(http::verb::get, std::string{ kChatMessagesRoute },
    [this](const auto& req, const auto& params) { return HandleGetMessageHistory(req, params); }
  );

  router.AddRoute(http::verb::get, std::string{ kMessageKeyRoute },
    [this](const auto& req, const auto& params) { return HandleGetMessagePublicKey(req, params); }
  );

  router.AddRoute(http::verb::post, std::string{ kReadMessageRoute },
    [this](const auto& req, const auto& params) { return HandleReadMessage(req, params); }
  );

  router.AddRoute(http::verb::get, std::string{ kGetUndeliveredRoute },
    [this](const auto& req, const auto& params) {return HandleGetUndelivered(req, params); }
  );

  router.AddRoute(http::verb::delete_, std::string{ kMessageIdRoute },
    [this](const auto& req, const auto& params) { return HandleDeleteMessage(req, params); }
  );
}
