#ifndef MESSAGE_PAYLOAD_VALIDATOR_H
#define MESSAGE_PAYLOAD_VALIDATOR_H

#include <variant>

#include "message_payload.h"

template <typename TMessagePayloadVariant>
constexpr bool AllMessagePayloadsValid = false;

template<typename ... TMessagePayload>
constexpr bool AllMessagePayloadsValid<std::variant<TMessagePayload...>> = (MessagePayload<TMessagePayload> && ...);

#endif // !MESSAGE_PAYLOAD_VALIDATOR_H
