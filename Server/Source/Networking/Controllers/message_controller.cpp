#include "message_controller.h"

#include "Networking/utils.h"
#include "Networking/api_errors.h"
#include "TypeLibHelpers/domain_class_type_variant_factory.h"
#include "Api/message_contract.h"
#include "boost/url/parse.hpp"

#include <chrono>

namespace {
  constexpr std::size_t kDefaultMsgHistoryPaginationLimit = 50;
  constexpr std::size_t kDefaultUndeliveredMsgPaginationLimit = 50;
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

    ChatId chat_id = ChatId::Reconstitute(netw::utils::ExtractPathParam(params, api::message::path_params::kChatId));

    auto body = nlohmann::json::parse(req.body());

    auto ciphertext = body.at(api::message::fields::kCiphertext).get<std::string>();

    auto msg_type = DomainClassTypeVariantFactory<MessageTypeVariant>::Create(
      body.at(api::message::fields::kType).get<std::string>()
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

    const ChatId chat_id = ChatId::Reconstitute(netw::utils::ExtractPathParam(params, api::message::path_params::kChatId));

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

    const MessageId msg_id = MessageId::Reconstitute(netw::utils::ExtractPathParam(params, api::message::path_params::kMessageId));

    auto encrypted_key = message_service_obs_->GetEncryptedKey(msg_id, *caller_id);

    if (!encrypted_key) {
      return api::errors::NotFound(req, "Encrypted key not found or you lack permission to access it.");
    }

    const nlohmann::json res_json = { {api::message::fields::kEncryptedKey, *encrypted_key} };

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

    const MessageId msg_id = MessageId::Reconstitute(netw::utils::ExtractPathParam(params, api::message::path_params::kMessageId));

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

    std::size_t limit = kDefaultUndeliveredMsgPaginationLimit;
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

    const MessageId msg_id = MessageId::Reconstitute(netw::utils::ExtractPathParam(params, api::message::path_params::kMessageId));

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
      {api::message::fields::kId, msg.GetId().ToString()},
      {api::message::fields::kSenderId, msg.GetSenderId().ToString()},
      {api::message::fields::kChatId, msg.GetChatId().ToString()},
      {api::message::fields::kCiphertext, msg.GetCiphertext()},
      {api::message::fields::kType, msg.GetTypeStr()},
      {api::message::fields::kCreatedAt, std::chrono::duration_cast<std::chrono::seconds>(msg.CreatedAt().time_since_epoch()).count()}
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

  for (const auto& [user_id_str, key_val] : reqBody.at(api::message::fields::kEncryptedKeys).items()) {
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

    if (const auto it = params.find(api::message::query_params::kLimit); it != params.end()) {
      limitOut = std::stoull(std::string((*it).value));
    }

    if (const auto it = params.find(api::message::query_params::kAfterId); it != params.end()) {
      afterIdOut = MessageId::Reconstitute(std::string((*it).value));
    }
  }
}

void MessageController::RegisterRoutes(Router& router) {
  router.AddRoute(http::verb::post, std::string{ api::message::routes::kChatMessages },
    [this](const auto& req, const auto& params) { return HandleSendMessage(req, params); }
  );

  router.AddRoute(http::verb::get, std::string{ api::message::routes::kChatMessages },
    [this](const auto& req, const auto& params) { return HandleGetMessageHistory(req, params); }
  );

  router.AddRoute(http::verb::get, std::string{ api::message::routes::kMessageKey },
    [this](const auto& req, const auto& params) { return HandleGetMessagePublicKey(req, params); }
  );

  router.AddRoute(http::verb::put, std::string{ api::message::routes::kReadMessage },
    [this](const auto& req, const auto& params) { return HandleReadMessage(req, params); }
  );

  router.AddRoute(http::verb::get, std::string{ api::message::routes::kGetUndelivered },
    [this](const auto& req, const auto& params) {return HandleGetUndelivered(req, params); }
  );

  router.AddRoute(http::verb::delete_, std::string{ api::message::routes::kMessageId },
    [this](const auto& req, const auto& params) { return HandleDeleteMessage(req, params); }
  );
}