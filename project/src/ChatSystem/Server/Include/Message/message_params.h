#ifndef MESSAGE_PARAMS_H
#define MESSAGE_PARAMS_H

#include "Payloads/message_payload_variant.h"

#include <Chat/chat_id.h>
#include <Message/message_id.h>
#include <Message/message_type.h>
#include <User/user_id.h>
#include <chrono>
#include <string>

/**
 * @brief Data transfer object for message creation and validation.
 *
 * MessageParams aggregates all data required to construct a Message.
 * It is typically passed to a validator before being used to create
 * a domain object.
 *
 * Validation rules (e.g. valid identifiers, content constraints, timestamps)
 * are enforced externally via an IValidator<MessageParams> implementation.
 */
struct MessageParams {
  // Required

  /// Unique identifier of the message.
  MessageId id;

  /// Identifier of the chat the message belongs to.
  ChatId chat_id;

  /// Identifier of the sender (must be valid).
  UserId sender_id;

  MessagePayloadVariant payload;

  /// Creation timestamp.
  std::chrono::system_clock::time_point created_at;

  // Optional
};

#endif  // MESSAGE_PARAMS_H