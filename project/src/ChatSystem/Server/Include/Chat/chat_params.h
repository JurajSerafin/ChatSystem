#ifndef CHAT_PARAMS_H
#define CHAT_PARAMS_H

#include "chat_id.h"

#include <chrono>
#include <optional>

struct ChatParams {
  ChatId id;

  std::chrono::system_clock::time_point created_at;

  std::optional<std::string> name;
};

#endif // CHAT_PARAMS_H