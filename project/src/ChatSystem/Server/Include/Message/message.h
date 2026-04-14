#ifndef MESSAGE_H
#define MESSAGE_H

#include <Chat/chat_id.h>
#include <Message/message_id.h>
#include <Message/message_params.h>
#include <Message/message_type.h>
#include <User/user_id.h>
#include <Validation/i_validator.h>
#include <chrono>
#include <stdexcept>
#include <string>

/**
 * @brief Represents a message in a chat system.
 *
 * The Message class encapsulates all data and state related to a single
 * chat message, including metadata such as sender, timestamps, delivery
 * state, and read state.
 *
 * Instances are immutable in identity but allow state transitions
 * (e.g. delivered/read flags).
 */
class Message {
public:
  using TimePoint = std::chrono::system_clock::time_point;

  /**
   * @brief Creates a validated Message instance.
   *
   * Validates the provided parameters using the given validator.
   * If validation fails, an exception is thrown.
   *
   * @param params Message parameters used for construction.
   * @param validator Validator used to verify correctness.
   * @return Constructed Message object.
   * @throws std::invalid_argument if validation fails.
   */
  [[nodiscard]] static Message Create(MessageParams params, const IValidator<MessageParams>& validator);

  Message(const Message&) = delete;
  Message& operator=(const Message&) = delete;

  Message(Message&&) = default;
  Message& operator=(Message&&) = default;

  /// @return Unique identifier of the message.
  [[nodiscard]] const MessageId& GetId() const;

  /// @return Identifier of the chat containing this message.
  [[nodiscard]] const ChatId& GetChatId() const;

  /// @return Identifier of the sender of the message.
  [[nodiscard]] const UserId& GetSenderId() const;

  /// @return Content/body of the message.
  [[nodiscard]] const std::string& GetContent() const;

  /// @return Type of the message (e.g. text, image, system).
  [[nodiscard]] MessageType GetType() const;

  /// @return Timestamp when the message was created.
  [[nodiscard]] TimePoint CreatedAt() const;

  /// @return True if the message has been read.
  [[nodiscard]] bool IsRead() const;

  /// @return True if the message has been delivered.
  [[nodiscard]] bool IsDelivered() const;

  /// Marks the message as read.
  void MarkRead();

  /// Marks the message as delivered.
  void MarkDelivered();

private:
  explicit Message::Message(MessageParams params)
    : id_{ std::move(params.id) },
    chat_id_{ std::move(params.chat_id) },
    sender_id_{ std::move(params.sender_id) },
    content_{ std::move(params.content) },
    type_{ params.type },
    created_at_{ params.created_at } {}

  MessageId id_;

  ChatId chat_id_;

  UserId sender_id_;

  std::string content_;

  MessageType type_;

  TimePoint created_at_;

  /// Indicates whether the message has been read by the recipient.
  bool is_read_{ false };

  /// Indicates whether the message has been delivered to the recipient.
  bool is_delivered_{ false };
};

inline Message Message::Create(MessageParams params, const IValidator<MessageParams>& validator) {

  if (const auto result = validator.Validate(params); !result.IsValid()) {
    throw std::invalid_argument{ result.Summary() };
  }

  return Message{ std::move(params) };
}

inline const MessageId& Message::GetId() const {
  return id_;
}

inline const ChatId& Message::GetChatId() const {
  return chat_id_;
}

inline const UserId& Message::GetSenderId() const {
  return sender_id_;
}

inline const std::string& Message::GetContent() const {
  return content_;
}

inline MessageType Message::GetType() const {
  return type_;
}

inline Message::TimePoint Message::CreatedAt() const {
  return created_at_;
}

inline bool Message::IsRead() const {
  return is_read_;
}

inline bool Message::IsDelivered() const {
  return is_delivered_;
}

inline void Message::MarkRead() {
  is_read_ = true;
}

inline void Message::MarkDelivered() {
  is_delivered_ = true;
}


#endif  // MESSAGE_H
