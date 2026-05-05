#ifndef I_MESSAGE_REPOSITORY_H
#define I_MESSAGE_REPOSITORY_H

#include <Message/message_id.h>
#include <User/user_id.h>

#include <vector>

class IMessageRepository {

  virtual void MarkDelivered(const MessageId& message_id, const UserId& recipient_id) = 0;

  virtual void MarkRead(const MessageId& message_id, const UserId& reader_id) = 0;

  virtual std::vector<UserId> GetReaders(const MessageId& message_id) = 0;

  virtual std::vector<UserId> GetRecipients(const MessageId& message_id) = 0;
};

#endif // I_MESSAgE_REPOSITORY_H
