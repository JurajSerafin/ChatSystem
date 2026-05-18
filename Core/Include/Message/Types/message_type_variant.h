#ifndef MESSAGE_TYPE_VARIANT_H
#define MESSAGE_TYPE_VARIANT_H

#include "message_type_validator.h"
#include "system_message_type.h"
#include "text_message_type.h"

#include <variant>

using MessageTypeVariant = std::variant<
  TextMessageType,
  SystemMessageType
>;

static_assert(AllMessageTypesValid<MessageTypeVariant>, "Payload type not valid, make sure it is compliant with MessagePayload concept.");

#endif // MESSAGE_TYPE_VARIANT_H
