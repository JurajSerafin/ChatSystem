#ifndef CHAT_TYPE_VALIDATOR_H
#define CHAT_TYPE_VALIDATOR_H

#include "chat_type.h"

#include <variant>

template <typename TChatTypeVariant>
constexpr bool AllChatTypesValid = false;

template<typename ... TChatTypes>
constexpr bool AllChatTypesValid<std::variant<TChatTypes ...>> = (ChatType<TChatTypes> && ...);

#endif // CHAT_TYPE_VALIDATOR_H
