#ifndef I_MESSAGE_SERVICE_H
#define I_MESSAGE_SERVICE_H

#include <string>
#include <vector>

#include <Chat/chat_id.h>
#include <User/user_id.h>
#include <Message/message_id.h>
#include <Message/message_type.h>
#include <Message/message.h>


class IMessageService {
public:
    virtual Message SendMessage(UserId senderId,
                                ChatId chatId,
                                const std::string& plaintext,
                                MessageType type) = 0;

    virtual std::vector<Message> GetHistory(ChatId chatId, int limit, int offset) = 0;

    virtual std::vector<Message> Search(ChatId chatId, const std::string& keywords) = 0;

    virtual void MarkAsRead(MessageId id, UserId readerId) = 0;

    virtual std::vector<Message> GetUndelivered(UserId userId) = 0;

    virtual void DeleteMessage(MessageId id, UserId requesterId) = 0;

};

#endif // I_MESSAGE_SERVICE_H