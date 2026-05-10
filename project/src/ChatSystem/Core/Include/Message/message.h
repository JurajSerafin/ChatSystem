#ifndef MESSAGE_H
#define MESSAGE_H

#include "Types/message_type_variant.h"

#include <Chat/chat_id.h>
#include <Message/message_id.h>
#include <Message/message_params.h>
#include <Message/message_validator_for.h>
#include <User/user_id.h>
#include <chrono>
#include <stdexcept>

/**
 * @brief Represents a message in a chat system.
 *
 * The Message class encapsulates all data and state related to a single
 * chat message, including metadata such as sender, timestamps, delivery
 * state, and read state, and strictly enforces its invariants on creation through
 * a generic validation policy provided at compile time.
 *
 * Instances are immutable in identity but allow state transitions (e.g. delivered/read flags).
 */
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
   * @tparam TMessageValidator Type of the validator used for Message entities.
   *
   * @param params Message parameters used for construction.
   * @param validator Validator used to verify correctness.
   * @return Constructed Message object.
   * @throws std::invalid_argument if validation fails.
   */
  template<MessageValidatorFor<MessageParams> TMessageValidator>
  [[nodiscard]] static Message Create(MessageParams params, const TMessageValidator& validator);

  [[nodiscard]] static Message Reconstitute(MessageParams params);

  /// @return Unique identifier of the message.
  [[nodiscard]] const MessageId& GetId() const;

  /// @return Identifier of the chat containing this message.
  [[nodiscard]] const ChatId& GetChatId() const;

  /// @return Identifier of the sender of the message.
  [[nodiscard]] const UserId& GetSenderId() const;

  /// @return Content/body of the message.
  [[nodiscard]] const MessageTypeVariant& GetType() const;

  [[nodiscard]] std::string_view GetCiphertext() const;

  [[nodiscard]] std::string GetTypeStr() const;

  /// @return Timestamp when the message was created.
  [[nodiscard]] std::chrono::system_clock::time_point CreatedAt() const;

private:
  /**
   * @brief Constructs a Message from parameters.
   * Use Create() for validated construction.
   * @param params Validated message parameters.
   */
  explicit Message(MessageParams params)
    : id_{std::move(params.id)},
    chat_id_{std::move(params.chat_id)},
    sender_id_{std::move(params.sender_id)},
    ciphertext_{std::move(params.ciphertext)},
    type_{params.type},
    created_at_{params.created_at},
    encrypted_keys_{std::move(params.encrypted_keys)} {}

  MessageId id_;

  ChatId chat_id_;

  UserId sender_id_;

  std::string ciphertext_;

  MessageTypeVariant type_;

  std::chrono::system_clock::time_point created_at_;

  std::unordered_map<UserId, std::string, BaseId<UserId>::Hasher> encrypted_keys_;

};
template<MessageValidatorFor<MessageParams> TMessageValidator>
Message Message::Create(MessageParams params, const TMessageValidator& validator) {
  if (const auto result = validator.Validate(params); !result.Ok()) {
    throw std::invalid_argument{ result.Summary() };
  }

  return Message{ std::move(params) };
}

inline Message Message::Reconstitute(MessageParams params) {
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

inline const MessageTypeVariant& Message::GetType() const {
  return type_;
}

inline std::chrono::system_clock::time_point Message::CreatedAt() const {
  return created_at_;
}

#endif  // MESSAGE_H
