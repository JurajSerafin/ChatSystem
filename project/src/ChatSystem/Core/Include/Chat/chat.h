#ifndef CHAT_H
#define CHAT_H

#include "Id/base_id.h"
#include "Types/chat_type_variant.h"
#include "chat_id.h"
#include "chat_params.h"
#include "chat_validator_for.h"

#include <Message/message.h>
#include <User/user_id.h>
#include <chrono>
#include <optional>
#include <stdexcept>
#include <vector>

/**
 * @brief Domain Aggregate Root representing a communication channel between users.
 * 
 * This class encapsulates the state of a chat, including its participants and metadata.
 * It enforces business rules, such as preventing participant removal in Direct Messages.
 */
class Chat {
public:
  /**
   * @brief Factory method to create a new Chat with domain validation.
   * 
   * @tparam TChatValidator A type satisfying the ChatValidatorFor concept.
   * 
   * @param params Initial state parameters for the chat.
   * @param validator The validator instance to check invariants.
   * @throws std::invalid_argument If parameters fail validation rules.
   * @return A validated Chat instance.
   */
  template<ChatValidatorFor<ChatParams> TChatValidator>
  [[nodiscard]] static Chat Create(ChatParams params, const TChatValidator& validator);

  /**
   * @brief Reconstitutes a Chat object from trusted storage (e.g., Database).
   * bypasses validation rules as the data is assumed to be valid.
   */
  [[nodiscard]] static Chat Reconstitute(ChatParams params);


  [[nodiscard]] const ChatId& GetId() const;

  [[nodiscard]] std::chrono::system_clock::time_point CreatedAt() const;

  [[nodiscard]] const ChatTypeVariant& GetType() const;

  [[nodiscard]] const std::optional<Message>& GetLastMessage() const;

  [[nodiscard]] const std::vector<UserId>& GetParticipantIds() const;

  /// @brief Adds a user with specified id to the chat participants.
  void AddParticipant(UserId userId);

  /**
   * @brief Removes a user from the chat.
   * 
   * @param userId The ID of the participant to remove.
   * @throws std::logic_error If the chat is a Direct Message (DM), as DMs have fixed participants.
   */
  void RemoveParticipant(const UserId& userId);

  /// @brief Updates the preview of the most recent message sent in this chat.
  void SetLastMessage(Message message);

private:
  ChatId id_;
  std::chrono::system_clock::time_point created_at_;
  ChatTypeVariant type_;
  std::optional<Message> last_message_;
  std::vector<UserId> participant_ids_;

  /// @brief Private constructor to enforce creation through factory methods.
  explicit Chat(ChatParams params);
};


template <ChatValidatorFor<ChatParams> TChatValidator>
Chat Chat::Create(ChatParams params, const TChatValidator& validator) {
  if (const auto result = validator.Validate(params); !result.Ok()) {
    throw std::invalid_argument{ result.Summary() };
  }
  return Chat{ std::move(params) };
}

inline Chat Chat::Reconstitute(ChatParams params) {
  return Chat{ std::move(params) };
}

inline Chat::Chat(ChatParams params)
  : id_(std::move(params.id)),
  created_at_(params.created_at),
  type_(std::move(params.type)),
  last_message_(std::move(params.last_message)),
  participant_ids_(std::move(params.participant_ids)) {
}


inline const ChatId& Chat::GetId() const {
  return id_;
}

inline std::chrono::system_clock::time_point Chat::CreatedAt() const {
  return created_at_;
}

inline const ChatTypeVariant& Chat::GetType() const {
  return type_;
}

inline const std::optional<Message>& Chat::GetLastMessage() const {
  return last_message_;
}

inline const std::vector<UserId>& Chat::GetParticipantIds() const {
  return participant_ids_;
}

inline void Chat::AddParticipant(UserId userId) {
  participant_ids_.emplace_back(std::move(userId));
}

inline void Chat::RemoveParticipant(const UserId& userId) {
  if (std::holds_alternative<DirectChatType>(type_)) {
    throw std::logic_error("Cannot remove participants from a Direct Message.");
  }

  std::erase(participant_ids_, userId);
}

inline void Chat::SetLastMessage(Message message) {
  last_message_ = std::move(message);
}

#endif // CHAT_H