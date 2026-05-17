#ifndef I_CLIENT_CHAT_SERVICE_H
#define I_CLIENT_CHAT_SERVICE_H

#include "Chat/chat.h"
#include "Chat/chat_id.h"
#include "Database/local_db_models.h"
#include "Message/message_id.h"

#include <vector>

class IClientChatService {
public:
  virtual ~IClientChatService() = default;

  virtual std::vector<CachedChat> GetChats(std::size_t limit, std::size_t offset) = 0;

  virtual CachedChat GetChatById(const ChatId& id) = 0;

  virtual void CreateChat(const std::vector<UserId>& participantIds) = 0;

  virtual std::vector<CachedUser> GetParticipants(const ChatId& chatId) = 0;
};

#endif // I_CLIENT_CHAT_SERVICE_H