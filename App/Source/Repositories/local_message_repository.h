#ifndef LOCAL_MESSAGE_REPOSITORY_H
#define LOCAL_MESSAGE_REPOSITORY_H

#include "Database/i_local_database.h"
#include "Repositories/i_local_message_repository.h"

/**
 * @brief Concrete implementation of the local message repository.
 * * Interacts with the local database to store, search, and retrieve
 * fully decrypted E2EE chat messages for immediate UI rendering.
 */
class LocalMessageRepository : public ILocalMessageRepository {
public:
  /**
   * @brief Constructs the repository with an injected database connection.
   * @param dbObs Pointer to the active local database instance.
   */
  explicit LocalMessageRepository(ILocalDatabase* dbObs);

  std::vector<CachedMessage> FindByChatId(std::string_view chatId, std::size_t limit, std::size_t offset) override;

  std::vector<CachedMessage> Search(std::string_view chatId, std::string_view keywords) override;

  void Delete(std::string_view messageId) override;

  void MarkAsRead(std::string_view messageId) override;

  void SaveForChat(const CachedMessage& message) override;

private:
  ILocalDatabase* db_obs_;
};

inline LocalMessageRepository::LocalMessageRepository(ILocalDatabase* dbObs) : db_obs_(dbObs) {}

#endif // LOCAL_MESSAGE_REPOSITORY_H