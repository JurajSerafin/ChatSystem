#ifndef I_MESSAGE_HANDLER_H
#define I_MESSAGE_HANDLER_H

#include <Message/message_type.h>
#include <Message/message.h>
#include <Message/raw_message.h>
#include <User/user.h>
#include <Chat/chat.h>


class IMessageHandler {
public:
    virtual MessageType HandledType() const = 0;

    virtual Message Handle(RawMessage const& raw, User const& sender, Chat const& chat) = 0;

    virtual ~IMessageHandler() = default;
};

#endif // I_MESSAGE_HANDLER_H