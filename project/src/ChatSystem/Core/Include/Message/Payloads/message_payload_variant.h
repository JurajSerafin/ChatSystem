#ifndef MESSAGE_CONTENT_H
#define MESSAGE_CONTENT_H

#include <variant>

#include "text_message_payload.h"
#include "system_message_payload.h"

using MessagePayloadVariant = std::variant<
  TextMessagePayload,
  SystemMessagePayload
>;

#endif // MESSAGE_CONTENT_H