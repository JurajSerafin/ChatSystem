#ifndef CORE_API_CHAT_CONTRACT_H
#define CORE_API_CHAT_CONTRACT_H

#include <string_view>

namespace api::chat {

  namespace routes {
    constexpr std::string_view kBase = "/chats";
    constexpr std::string_view kById = "/chats/{id}";
    constexpr std::string_view kParticipants = "/chats/{id}/participants";
    constexpr std::string_view kDeleteParticipant = "/chats/{id}/participants/{user_id}";
  }

  namespace path_params {
    constexpr std::string_view kChatId = "id";
    constexpr std::string_view kUserId = "user_id";
  }

  // JSON Payload and Response Fields
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
  }

} // namespace api::chat

#endif // CORE_API_CHAT_CONTRACT_H