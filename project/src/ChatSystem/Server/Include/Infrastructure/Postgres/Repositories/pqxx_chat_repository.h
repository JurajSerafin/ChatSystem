#ifndef PQXX_CHAT_REPOSITORY_H
#define PQXX_CHAT_REPOSITORY_H

#include <Repositories/i_chat_repository.h>
#include <Database/i_connection_pool.h>

class PqxxChatRepository : public IChatRepository {
public:
  explicit PqxxChatRepository(IConnectionPool* connectionPoolObs);

  Chat Create(const Chat& chat) override;

  std::optional<Chat> FindById(const ChatId& id) override;

  std::vector<Chat> FindByUserId(const UserId& userId, std::size_t limit, std::size_t offset) override;

  void AddParticipant(const ChatId& chatId, const UserId& userId) override;

  void RemoveParticipant(const ChatId& chatId, const UserId& userId) override;

  bool IsParticipant(const ChatId& chatId, const UserId& userId) override;

  void UpdateLastMessage(const ChatId& chatId, const Message& message) override;

  void DeleteById(const ChatId& id) override;

private:
  IConnectionPool* connection_pool_obs_;
};

#endif // PQXX_CHAT_REPOSITORY_H