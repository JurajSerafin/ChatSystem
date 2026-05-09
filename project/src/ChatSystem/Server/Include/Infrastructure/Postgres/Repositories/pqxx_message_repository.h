#ifndef PQXX_MESSAGE_REPOSITORY_H
#define PQXX_MESSAGE_REPOSITORY_H

#include <Repositories/i_message_repository.h>

class IConnectionPool;

class PqxxMessageRepository : public IMessageRepository {
public:

  explicit PqxxMessageRepository(IConnectionPool* connectionPoolObs);

  [[nodiscard]] std::optional<Message> FindById(const MessageId& id) override;

  [[nodiscard]] std::vector<Message> FindByChatId(const ChatId& chatId, std::size_t limit, std::size_t offset) override;

  [[nodiscard]] std::vector<Message> Search(const ChatId& chatId, const std::string& keywords, std::size_t limit, std::size_t offset) override;

  [[nodiscard]] std::vector<Message> FindUndelivered(const UserId& recipientId) override;

  [[nodiscard]] std::vector<UserId> GetReaders(const MessageId& messageId) override;

  [[nodiscard]] std::vector<UserId> GetDeliveredTo(const MessageId& messageId) override;

  void DeleteById(const MessageId& id) override;

  void MarkDelivered(const MessageId& messageId, const UserId& recipientId) override;

  void MarkRead(const MessageId& messageId, const UserId& readerId) override;

  Message Save(Message message) override;

private:
  IConnectionPool* connection_pool_obs_;
};

#endif // PQXX_MESSAGE_REPOSITORY_H