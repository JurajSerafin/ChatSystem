#ifndef I_CHAT_REPOSITORY_H
#define I_CHAT_REPOSITORY_H

#include <vector>
#include <optional>

#include <Chat/chat.h>
#include <Chat/chat_id.h>
#include <User/user_id.h>
#include <User/user.h>
#include <Message/message_id.h>

class IChatRepository {
public:

  virtual Chat Create(const Chat& chat) = 0;

  virtual std::optional<Chat> FindById(ChatId id) = 0;

  virtual std::vector<Chat> FindByUserId(UserId userId) = 0;

  virtual std::vector<User> GetParticipants(ChatId id) = 0;

  virtual void AddParticipant(ChatId chatId, UserId userId) = 0;

  virtual void RemoveParticipant(ChatId chatId, UserId userId) = 0;

  virtual void UpdateLastMessage(ChatId chatId, MessageId messageId) = 0;

  virtual void DeleteById(ChatId id) = 0;
};

#endif // I_CHAT_REPOSITORY_H
