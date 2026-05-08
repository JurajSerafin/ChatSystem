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

class Chat {
private:
  ChatId id_;

  std::chrono::system_clock::time_point created_at_;

  ChatTypeVariant type_;

  std::optional<Message> last_message_;

  std::vector<UserId> participant_ids_;

  explicit Chat(ChatParams params);

public:
  template<ChatValidatorFor<ChatParams> TChatValidator>
  [[nodiscard]] static Chat Create(ChatParams params, const TChatValidator& validator);

  [[nodiscard]] static Chat Reconstitute(ChatParams params);

  [[nodiscard]] const ChatId& GetId() const;

  [[nodiscard]] std::chrono::system_clock::time_point CreatedAt() const;

  [[nodiscard]] const ChatTypeVariant& GetType() const;

  [[nodiscard]] const std::optional<Message>& GetLastMessage() const;

  [[nodiscard]] const std::vector<UserId>& GetParticipantIds() const;

  void SetLastMessage(Message message);
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


inline void Chat::SetLastMessage(Message message) {
  last_message_ = std::move(message);
}

#endif // CHAT_H