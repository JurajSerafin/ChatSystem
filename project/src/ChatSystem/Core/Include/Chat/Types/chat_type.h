#ifndef CHAT_TYPE_H
#define CHAT_TYPE_H

#include <concepts>
#include <stdexcept>
#include <string_view>

template<typename T>
concept ChatType = requires(T chatType) {
  { T::TypeString() } -> std::convertible_to<std::string_view>;
  { chatType.IsValid() } -> std::same_as<bool>;
};

#endif // CHAT_TYPE_H

