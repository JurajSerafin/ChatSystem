#ifndef I_CLIENT_MESSAGE_SERVICE_H
#define I_CLIENT_MESSAGE_SERVICE_H

#include "Chat/chat_id.h"
#include "Database/local_db_models.h"
#include "Message/message_id.h"

class IClientMessageService {
public:
  virtual ~IClientMessageService() = default;

  virtual void SendMessage(const ChatId& chatId, std::string_view plainText) = 0;

  virtual std::vector<CachedMessage> GetHistory(const ChatId& chatId, std::size_t limit, std::size_t offset) = 0;

  virtual void MarkAsRead(const MessageId& messageId) = 0;

  virtual std::vector<CachedMessage> GetUndelivered() = 0;

  virtual std::vector<CachedMessage> SearchMessages(const ChatId& chatId, std::string_view keywords) = 0;
};


#endif // I_CLIENT_MESSAGE_SERVICE_H