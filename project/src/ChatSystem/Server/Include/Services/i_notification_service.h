#ifndef I_NOTIFICATION_SERVICE_H
#define I_NOTIFICATION_SERVICE_H

#include <User/user_id.h>
#include <Message/message_id.h>
#include <Message/message.h>


class INotificationService {
public:
    virtual void NotifyNewMessage(UserId recipientId, const Message& message) = 0;

    virtual void NotifyReadReceipt(UserId senderId, MessageId messageId, UserId readerId) = 0;

    virtual void NotifyUserOnline(UserId userId) = 0;
    virtual void NotifyUserOffline(UserId userId) = 0;
};

#endif // I_NOTIFICATION_SERVICE_H
