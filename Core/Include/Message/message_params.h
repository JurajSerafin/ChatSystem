#ifndef MESSAGE_PARAMS_H
#define MESSAGE_PARAMS_H

#include "Types/message_type_variant.h"

#include <Chat/chat_id.h>
#include <Message/message_id.h>
#include <User/user_id.h>
#include <chrono>
#include <string>
#include <unordered_map>

/**
 * @brief DTO for message creation and validation.
 *
 * MessageParams aggregates all data required to construct a Message.
 */
struct MessageParams {
  // Required

  MessageId id;

  ChatId chat_id;

  UserId sender_id;

  std::string ciphertext;

  MessageTypeVariant type;

  std::chrono::system_clock::time_point created_at;

  // Optional
};

#endif  // MESSAGE_PARAMS_H