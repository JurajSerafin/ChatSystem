#ifndef MESSAGE_TYPE_H
#define MESSAGE_TYPE_H

#include <concepts>
#include <string_view>

template<typename T>
concept MessageType = requires(T payload) {
  { T::TypeString() } -> std::convertible_to<std::string_view>;
};

#endif // MESSAGE_TYPE_H
