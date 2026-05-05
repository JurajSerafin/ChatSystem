#ifndef MESSAGE_PAYLOAD_FACTORY_H
#define MESSAGE_PAYLOAD_FACTORY_H

#include <variant>
#include <format>

#include "message_payload.h"


template<typename TMessagePayload>
class MessagePayloadFactory;

template<MessagePayload ... TMessagePayloads>
class MessagePayloadFactory <std::variant<TMessagePayloads ...>> {
public:
  [[nodiscard]] static std::variant<TMessagePayloads ...> Create(std::string_view typeStr, std::string content);
};


template <MessagePayload... TMessagePayloads>
std::variant<TMessagePayloads...> MessagePayloadFactory<std::variant<TMessagePayloads...>>::Create(
  std::string_view typeStr,
  std::string content
) {
  std::variant<TMessagePayloads...> payload_variant;

  bool payload_exists = (
    (TMessagePayloads::TypeString() == typeStr
      ? (payload_variant = TMessagePayloads{std::move(content)}, true)
      : false
    ) || ...
  );

  if (!payload_exists) {
    throw std::invalid_argument(std::format("Unable to recognize payload: Unknown payload type string '{}'", typeStr));
  }

  return payload_variant;
}

#endif // MESSAGE_PAYLOAD_FACTORY_H
