#ifndef CORE_API_CHAT_CONTRACT_H
#define CORE_API_CHAT_CONTRACT_H

#include <string_view>

/**
 * @brief API contract definitions for chat room management.
 * Contains URL routes, path parameters, and JSON fields for creating and interacting with chat entities.
 */
namespace api::chat {

  /** @brief HTTP routing paths for chat operations. */
  namespace routes {
    /// POST (create) or GET (list) chat rooms.
    constexpr std::string_view kBase = "/chats";

    /// GET or DELETE a specific chat room by its ID.
    constexpr std::string_view kById = "/chats/{id}";

    /// GET (list) or POST (add) participants in a specific chat.
    constexpr std::string_view kParticipants = "/chats/{id}/participants";

    /// DELETE a specific user from a specific chat.
    constexpr std::string_view kDeleteParticipant = "/chats/{id}/participants/{user_id}";

  } // namespace api::chat::routes

  /** @brief Template parameters used in URL path substitution. */
  namespace path_params {
    /// The template token representing a chat room's unique ID.
    constexpr std::string_view kChatId = "id";

    /// The template token representing a user's unique ID.
    constexpr std::string_view kUserId = "user_id";

  } // namespace api::chat::path_params

  /** @brief JSON payload keys used in chat-related requests and responses. */
  namespace fields {
    constexpr std::string_view kParticipantIds = "participant_ids";

    constexpr std::string_view kId = "id";

    constexpr std::string_view kCreatedAt = "created_at";

    constexpr std::string_view kUserId = "user_id";

    constexpr std::string_view kUsername = "login";

    constexpr std::string_view kTag = "tag";

    constexpr std::string_view kLastMessageId = "last_message_id";

    constexpr std::string_view kName = "name";

    constexpr std::string_view kLastActivityAt = "last_activity_at";

  } // namespace api::chat::fields

} // namespace api::chat

#endif // CORE_API_CHAT_CONTRACT_H