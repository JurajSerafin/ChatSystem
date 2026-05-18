#ifndef CHAT_PARAMS_H
#define CHAT_PARAMS_H

#include "Message/message.h"
#include "Types/chat_type_variant.h"
#include "chat_id.h"

#include <chrono>
#include <optional>

/**
 * @brief DTO for transferring Chat state.
 * Used primarily for construction and reconstitution of Chat entities.
 */
struct ChatParams {
  ChatId id;

  std::chrono::system_clock::time_point created_at;

  ChatTypeVariant type;

  std::optional<Message> last_message;

  std::vector<UserId> participant_ids;
};

#endif // CHAT_PARAMS_H