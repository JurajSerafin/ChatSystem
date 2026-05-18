#ifndef PQXX_CHAT_REPOSITORY_H
#define PQXX_CHAT_REPOSITORY_H

#include <Repositories/i_chat_repository.h>
#include <Database/i_connection_pool.h>

/**
 * @brief Concrete PostgreSQL implementation of the server-side chat repository.
 * * Uses the injected connection pool to execute highly concurrent transactions
 * managing chat metadata and the many-to-many participant junction tables.
 */
class PqxxChatRepository : public IChatRepository {
public:
  /**
   * @brief Constructs the repository with a thread-safe connection pool.
   * @param connectionPoolObs Observer pointer to the active PostgreSQL connection pool.
   */
  explicit PqxxChatRepository(IConnectionPool* connectionPoolObs);

  void Add(const Chat& chat) override;

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