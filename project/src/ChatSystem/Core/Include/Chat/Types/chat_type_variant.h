#ifndef CHAT_TYPE_VARIANT_H
#define CHAT_TYPE_VARIANT_H

#include "chat_type_validator.h"
#include "direct_chat_type.h"

#include <variant>

using ChatTypeVariant = std::variant<
  DirectChatType
>;

static_assert(AllChatTypesValid<ChatTypeVariant>, "Chat type not valid, make sure it is compliant with ChatType concept.");


#endif // CHAT_TYPE_VARIANT_H
