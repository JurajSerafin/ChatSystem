#ifndef MESSAGE_CONTENT_H
#define MESSAGE_CONTENT_H

#include "message_payload_validator.h"
#include "system_message_payload.h"
#include "text_message_payload.h"

#include <variant>

using MessagePayloadVariant = std::variant<
  TextMessagePayload,
  SystemMessagePayload
>;

static_assert(AllMessagePayloadsValid<MessagePayloadVariant>, "Payload type not valid, make sure it is compliant with MessagePayload concept.");

#endif // MESSAGE_CONTENT_H
