#include "db_model_json_mapper.h"

#include "Api/auth_contract.h"
#include "Api/user_contract.h"
#include "Api/chat_contract.h"
#include "Api/message_contract.h"

#include <chrono>

namespace {
  std::chrono::system_clock::time_point ParseTimestamp(int64_t epoch_seconds) {
    return std::chrono::system_clock::from_time_t(epoch_seconds);
  }
} // namespace

CachedIdentity DBModelJsonMapper::ToIdentity(const nlohmann::json& j) {
  CachedIdentity identity;

  identity.id = j.at(api::user::fields::kId).get<std::string>();

  identity.login = j.at(api::user::fields::kLogin).get<std::string>();

  identity.tag = j.at(api::user::fields::kTag).get<std::string>();

  identity.session_token = j.at(api::auth::fields::kToken).get<std::string>();

  return identity;
}

CachedUser DBModelJsonMapper::ToUser(const nlohmann::json& j) {
  CachedUser user;

  user.id = j.at(api::user::fields::kId).get<std::string>();

  user.login = j.at(api::user::fields::kLogin).get<std::string>();

  user.tag = j.at(api::user::fields::kTag).get<std::string>();

  user.public_key = j.at(api::user::fields::kPublicKey).get<std::string>();

  user.cached_at = std::chrono::system_clock::now();

  user.is_deleted = false;

  return user;
}

CachedChat DBModelJsonMapper::ToChat(const nlohmann::json& j) {
  CachedChat chat;
  chat.id = j.at(api::chat::fields::kId).get<std::string>();

  if (j.contains(api::chat::fields::kLastMessageId) && !j.at(api::chat::fields::kLastMessageId).is_null()) {
    chat.last_message_id = j.at(api::chat::fields::kLastMessageId).get<std::string>();
  }

  if (j.contains(api::chat::fields::kName) && !j.at(api::chat::fields::kName).is_null()) {
    chat.name = j.at(api::chat::fields::kName).get<std::string>();
  }

  int64_t activity_epoch = j.at(api::chat::fields::kLastActivityAt).get<int64_t>();

  chat.last_activity_at = ParseTimestamp(activity_epoch);

  chat.cached_at = std::chrono::system_clock::now();

  chat.is_deleted = false;

  return chat;
}

CachedMessage DBModelJsonMapper::ToMessage(const nlohmann::json& j, std::string plaintext) {
  CachedMessage msg;

  msg.id = j.at(api::message::fields::kId).get<std::string>();

  msg.sender_id = j.at(api::message::fields::kSenderId).get<std::string>();

  msg.chat_id = j.at(api::message::fields::kChatId).get<std::string>();

  msg.type = j.at(api::message::fields::kType).get<std::string>();

  msg.plaintext = std::move(plaintext);

  int64_t created_epoch = j.at(api::message::fields::kCreatedAt).get<int64_t>();

  msg.created_at = ParseTimestamp(created_epoch);

  msg.is_delivered = true;
  msg.is_read = false;
  msg.is_deleted = false;

  return msg;
}