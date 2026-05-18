#ifndef CORE_API_MESSAGE_CONTRACT_H
#define CORE_API_MESSAGE_CONTRACT_H

#include <string_view>

/**
 * @brief API contract definitions for End-to-End Encrypted (E2EE) messaging.
 * Contains URL routes and fields for transmitting ciphertexts and wrapped keys.
 */
namespace api::message {

  /** @brief HTTP routing paths for messaging operations. */
  namespace routes {
    /// GET (history) or POST (send) messages in a specific chat.
    constexpr std::string_view kChatMessages = "/chats/{id}/messages";

    /// GET the wrapped symmetric key for a specific message targeted to the active user.
    constexpr std::string_view kMessageKey = "/messages/{id}/key";

    /// POST/PUT to mark a specific message as read by the active user.
    constexpr std::string_view kReadMessage = "/messages/{id}/read";

    /// GET all pending messages that have not yet been delivered to the active user.
    constexpr std::string_view kGetUndelivered = "/messages/undelivered";

    /// GET, PUT, or DELETE a specific message entity.
    constexpr std::string_view kMessageId = "/messages/{id}";

  } // namespace api::message::routes

  /** @brief Template parameters used in URL path substitution. */
  namespace path_params {

    constexpr std::string_view kChatId = "id";

    constexpr std::string_view kMessageId = "id";

  } // namespace api::message::path_params

  /** @brief Query parameters used for paginated or filtered requests. */
  namespace query_params {

    constexpr std::string_view kLimit = "limit";

    constexpr std::string_view kOffset = "offset";

    constexpr std::string_view kAfterId = "afterId";

  } // namespace api::message::query_params

  /** @brief JSON payload keys used in message-related requests and responses. */
  namespace fields {

    constexpr std::string_view kId = "id";

    constexpr std::string_view kSenderId = "sender_id";

    constexpr std::string_view kChatId = "chat_id";

    constexpr std::string_view kCiphertext = "ciphertext";

    constexpr std::string_view kEncryptedKeys = "encrypted_keys";

    constexpr std::string_view kEncryptedKey = "encrypted_key";

    constexpr std::string_view kType = "type";

    constexpr std::string_view kCreatedAt = "created_at";
  } // namespace api::message::fields

} // namespace api::message

#endif // CORE_API_MESSAGE_CONTRACT_H