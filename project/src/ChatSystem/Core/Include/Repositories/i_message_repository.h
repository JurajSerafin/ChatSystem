#ifndef I_MESSAGE_REPOSITORY_H
#define I_MESSAGE_REPOSITORY_H

#include "Message/encrypted_keys_map.h"
#include "Message/message.h"

#include <Message/message_id.h>
#include <User/user_id.h>
#include <vector>

class IMessageRepository {
public:
  [[nodiscard]] virtual std::optional<Message> FindById(const MessageId& id) = 0;

  [[nodiscard]] virtual std::vector<Message> FindByChatId(const ChatId& chatId, std::size_t limit, std::size_t offset) = 0;

  [[nodiscard]] virtual std::vector<Message> FindUndelivered(const UserId& recipientId) = 0;

  [[nodiscard]] virtual std::vector<UserId> GetReaders(const MessageId& messageId) = 0;

  [[nodiscard]] virtual std::vector<UserId> GetDeliveredTo(const MessageId& messageId) = 0;

  virtual void DeleteById(const MessageId& id) = 0;

  virtual void MarkDelivered(const MessageId& messageId, const UserId& recipientId) = 0;

  virtual void MarkRead(const MessageId& messageId, const UserId& readerId) = 0;

  virtual Message Save(Message message, const EncryptedKeysMap& encryptedKeys) = 0;

  virtual std::optional<std::string> GetEncryptedKey(const MessageId& messageId, const UserId& userId) = 0;

  virtual ~IMessageRepository() = default;

};

#endif // I_MESSAGE_REPOSITORY_H
