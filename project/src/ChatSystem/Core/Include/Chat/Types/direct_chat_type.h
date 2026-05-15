#ifndef DIRECT_CHAT_TYPE_H
#define DIRECT_CHAT_TYPE_H

#include <string_view>

/**
 * @struct DirectChatType
 * @brief Represents a standard 1-on-1 private conversation.
 */
struct DirectChatType {
  /// @brief Returns a string token representation for database serialization and logging.
  static constexpr std::string_view TypeString() noexcept {
    return "DIRECT";
  }

  static constexpr bool IsValid() noexcept {
    return !TypeString().empty();
  }
};


#endif // DIRECT_CHAT_TYPE_H
