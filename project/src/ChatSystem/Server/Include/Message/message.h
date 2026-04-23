#ifndef MESSAGE_H
#define MESSAGE_H

#include <Chat/chat_id.h>
#include <Message/message_id.h>
#include <Message/message_params.h>
#include <Message/message_type.h>
#include <Message/message_validator.h>
#include <User/user_id.h>
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
 * * @tparam TMessageParamsValidator The validator policy type ensuring data integrity.
 */
template<MessageValidatorFor<MessageParams> TMessageParamsValidator>
class Message {
public:
  /**
   * @brief Default move constructor.
   * Efficiently transfers ownership of a message's data.
   */
  Message(Message&&) = default;

  /**
   * @brief Default move assignment operator.
   * Efficiently transfers ownership of message data to an existing instance.
   */
  Message& operator=(Message&&) = default;

  /**
   * @brief Deleted copy constructor.
   * Copying is strictly disabled. A message entity represents a unique
   * historical record in the database. Duplicating it would break identity semantics.
   */
  Message(const Message&) = delete;

  /**
   * @brief Deleted copy assignment operator.
   * Copy assignment is strictly disabled to prevent overwriting an existing
   * message record with another message's data.
   */
  Message& operator=(const Message&) = delete;

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
  [[nodiscard]] static Message<TMessageParamsValidator> Create(MessageParams params, const TMessageParamsValidator& validator);

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
  [[nodiscard]] std::chrono::system_clock::time_point CreatedAt() const;

  /// @return True if the message has been read.
  [[nodiscard]] bool IsRead() const;

  /// @return True if the message has been delivered.
  [[nodiscard]] bool IsDelivered() const;

  /// Marks the message as read.
  void MarkRead();

  /// Marks the message as delivered.
  void MarkDelivered();

private:
  /**
   * @brief Constructs a Message from parameters.
   * Use Create() for validated construction.
   * @param params Validated message parameters.
   */
  explicit Message(MessageParams params)
    : id_{ std::move(params.id) },
    chat_id_{ std::move(params.chat_id) },
    sender_id_{ std::move(params.sender_id) },
    content_{ std::move(params.content) },
    type_{ params.type },
    created_at_{ params.created_at } {
  }

  MessageId id_;

  ChatId chat_id_;

  UserId sender_id_;

  std::string content_;

  MessageType type_;

  std::chrono::system_clock::time_point created_at_;

  /// Indicates whether the message has been read by the recipient.
  bool is_read_{ false };

  /// Indicates whether the message has been delivered to the recipient.
  bool is_delivered_{ false };
};

template<MessageValidatorFor<MessageParams> TMessageParamsValidator>
Message<TMessageParamsValidator> Message<TMessageParamsValidator>::Create(MessageParams params, const TMessageParamsValidator& validator) {

  // FIX: Changed result.IsValid() to result.Ok()
  if (const auto result = validator.Validate(params); !result.Ok()) {
    throw std::invalid_argument{ result.Summary() };
  }

  return Message{ std::move(params) };
}

template<MessageValidatorFor<MessageParams> TMessageParamsValidator>
const MessageId& Message<TMessageParamsValidator>::GetId() const {
  return id_;
}

template<MessageValidatorFor<MessageParams> TMessageParamsValidator>
const ChatId& Message<TMessageParamsValidator>::GetChatId() const {
  return chat_id_;
}

template<MessageValidatorFor<MessageParams> TMessageParamsValidator>
const UserId& Message<TMessageParamsValidator>::GetSenderId() const {
  return sender_id_;
}

template<MessageValidatorFor<MessageParams> TMessageParamsValidator>
const std::string& Message<TMessageParamsValidator>::GetContent() const {
  return content_;
}

template<MessageValidatorFor<MessageParams> TMessageParamsValidator>
MessageType Message<TMessageParamsValidator>::GetType() const {
  return type_;
}

template<MessageValidatorFor<MessageParams> TMessageParamsValidator>
std::chrono::system_clock::time_point Message<TMessageParamsValidator>::CreatedAt() const {
  return created_at_;
}

template<MessageValidatorFor<MessageParams> TMessageParamsValidator>
bool Message<TMessageParamsValidator>::IsRead() const {
  return is_read_;
}

template<MessageValidatorFor<MessageParams> TMessageParamsValidator>
bool Message<TMessageParamsValidator>::IsDelivered() const {
  return is_delivered_;
}

template<MessageValidatorFor<MessageParams> TMessageParamsValidator>
void Message<TMessageParamsValidator>::MarkRead() {
  is_read_ = true;
}

template<MessageValidatorFor<MessageParams> TMessageParamsValidator>
void Message<TMessageParamsValidator>::MarkDelivered() {
  is_delivered_ = true;
}

#endif  // MESSAGE_H