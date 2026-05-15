#ifndef DIRECT_CHAT_TYPE_H
#define DIRECT_CHAT_TYPE_H

#include <string_view>

struct DirectChatType {
  static constexpr std::string_view TypeString() noexcept {
    return "DIRECT";
  }

  static constexpr bool IsValid() noexcept {
    return !TypeString().empty();
  }
};


#endif // DIRECT_CHAT_TYPE_H
