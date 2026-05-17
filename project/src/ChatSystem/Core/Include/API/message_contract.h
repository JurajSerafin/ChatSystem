#ifndef CORE_API_MESSAGE_CONTRACT_H
#define CORE_API_MESSAGE_CONTRACT_H

#include <string_view>

namespace api::message {

  namespace routes {
    constexpr std::string_view kChatMessages = "/chats/{id}/messages";
    constexpr std::string_view kMessageKey = "/messages/{id}/key";
    constexpr std::string_view kReadMessage = "/messages/{id}/read";
    constexpr std::string_view kGetUndelivered = "/messages/undelivered";
    constexpr std::string_view kMessageId = "/messages/{id}";
  }

  namespace path_params {
    constexpr std::string_view kChatId = "id";
    constexpr std::string_view kMessageId = "id";
  }

  namespace query_params {
    constexpr std::string_view kLimit = "limit";
    constexpr std::string_view kOffset = "offset";
    constexpr std::string_view kAfterId = "afterId";
  }

  // JSON Payload & Response Fields
  namespace fields {
    constexpr std::string_view kId = "id";
    constexpr std::string_view kSenderId = "sender_id";
    constexpr std::string_view kChatId = "chat_id";
    constexpr std::string_view kCiphertext = "ciphertext";
    constexpr std::string_view kEncryptedKeys = "encrypted_keys";
    constexpr std::string_view kEncryptedKey = "encrypted_key";
    constexpr std::string_view kType = "type";
    constexpr std::string_view kCreatedAt = "created_at";
  }

} // namespace Api::Message

#endif // CORE_API_MESSAGE_CONTRACT_H