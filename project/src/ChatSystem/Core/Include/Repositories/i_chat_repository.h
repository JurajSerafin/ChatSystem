#ifndef I_CHAT_REPOSITORY_H
#define I_CHAT_REPOSITORY_H

#include <Chat/chat.h>
#include <Chat/chat_id.h>
#include <User/user_id.h>
#include <optional>
#include <vector>

class IChatRepository {
public:
  virtual ~IChatRepository() = default;

  virtual Chat Create(const Chat& chat) = 0;

  virtual std::optional<Chat> FindById(const ChatId& id) = 0;

  virtual std::vector<Chat> FindByUserId(const UserId& userId, std::size_t limit, std::size_t offset) = 0;

  virtual void AddParticipant(const ChatId& chatId, const UserId& userId) = 0;

  virtual void RemoveParticipant(const ChatId& chatId, const UserId& userId) = 0;

  virtual bool IsParticipant(const ChatId& chatId,  const UserId& userId) = 0;

  virtual void UpdateLastMessage(const ChatId& chatId,  const Message& message) = 0;

  virtual void DeleteById(const ChatId& id) = 0;
};

#endif // I_CHAT_REPOSITORY_H
