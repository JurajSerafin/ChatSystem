#ifndef MESSAGE_TYPE_VALIDATOR_H
#define MESSAGE_TYPE_VALIDATOR_H

#include "message_type.h"

#include <variant>

template <typename TMessagePayloadVariant>
constexpr bool AllMessageTypesValid = false;

template<typename ... TMessagePayload>
constexpr bool AllMessageTypesValid<std::variant<TMessagePayload...>> = (MessageType<TMessagePayload> && ...);

#endif // MESSAGE_TYPE_VALIDATOR_H
