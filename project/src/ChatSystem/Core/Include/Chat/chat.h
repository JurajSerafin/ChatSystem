#ifndef CHAT_H
#define CHAT_H

#include "Id/base_id.h"
#include "chat_id.h"
#include "chat_params.h"
#include "chat_validator.h"

#include <Message/message.h>
#include <chrono>
#include <optional>
#include <shared_mutex>
#include <string>

class Chat {
private:
  
  ChatId id_;

  std::chrono::system_clock::time_point created_at_;

  std::optional<std::string> name_;

  explicit Chat(ChatParams params);

public:

  template<ChatValidatorFor<ChatParams> TChatValidator>
  [[nodiscard]] static Chat Create(ChatParams params, const TChatValidator& validator);

  [[nodiscard]] static Chat Reconstitute(ChatParams params);

  [[nodiscard]] const ChatId& GetId() const;

  [[nodiscard]] std::chrono::system_clock::time_point CreatedAt() const;

  [[nodiscard]] const std::optional<std::string>& GetName() const;

  void SetName(std::string name);
  
};

template <ChatValidatorFor<ChatParams> TChatValidator>
Chat Chat::Create(ChatParams params, const TChatValidator& validator) {
  if (const auto result = validator.Validate(params); !result.Ok()) {
    throw std::invalid_argument{ result.Summary() };
  }

  return Chat{ std::move(params) };
}

inline Chat::Chat(ChatParams params) : id_(std::move(params.id)), created_at_(params.created_at), name_(params.name) {}

inline Chat Chat::Reconstitute(ChatParams params) {
  return Chat{ std::move(params) };
}
inline const ChatId& Chat::GetId() const {
  return id_;
}
inline std::chrono::system_clock::time_point Chat::CreatedAt() const {
  return created_at_;
}
inline const std::optional<std::string>& Chat::GetName() const {
  return name_;
}

#endif // CHAT_H