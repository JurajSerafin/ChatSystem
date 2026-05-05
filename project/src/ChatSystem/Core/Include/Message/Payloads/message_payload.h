#ifndef MESSAGE_PAYLOAD_H
#define MESSAGE_PAYLOAD_H

#include <concepts>
#include <string_view>
#include <variant>
#include <stdexcept>

template<typename T>
concept MessagePayload = requires(T payload) {
  { T::TypeString() } -> std::convertible_to<std::string_view>;
  { payload.GetContent() } -> std::convertible_to<std::string_view>;
  { payload.IsValid() } -> std::convertible_to<bool>;
};

#endif // !MESSAGE_PAYLOAD_H
