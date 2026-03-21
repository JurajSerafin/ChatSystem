#ifndef I_MESSAGE_HANDLER_H
#define I_MESSAGE_HANDLER_H

#include <Chat/chat.h>
#include <Message/message.h>
#include <Message/message_type.h>
#include <Message/raw_message.h>
#include <User/user.h>

class IMessageHandler {
public:
    virtual MessageType HandledType() const = 0;

    virtual Message Handle(const RawMessage& raw, const User& sender, const Chat& chat) = 0;

    virtual ~IMessageHandler() = default;
};

#endif  // I_MESSAGE_HANDLER_H