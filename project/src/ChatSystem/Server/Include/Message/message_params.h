#ifndef MESSAGE_PARAMS_H
#define MESSAGE_PARAMS_H

#include <Chat/chat_id.h>
#include <Message/message_id.h>
#include <Message/message_type.h>
#include <User/user_id.h>
#include <chrono>
#include <string>

/**
 * @brief Data transfer object containing message parameters.
 *
 * Holds all required data to create or process a message within a chat.
 * Typically used as input for validation and messaging logic.
 */
struct MessageParams {
  // Required

  /// Unique identifier of the message.
  MessageId id;

  /// Identifier of the chat the message belongs to.
  ChatId chat_id;

  /// Identifier of the user who sent the message.
  UserId sender_id;

  /// Message content.
  std::string content;

  /// Type of the message.
  MessageType type;

  /// Timestamp when the message was created.
  std::chrono::system_clock::time_point created_at;

  // Optional
};

#endif  // MESSAGE_PARAMS_H