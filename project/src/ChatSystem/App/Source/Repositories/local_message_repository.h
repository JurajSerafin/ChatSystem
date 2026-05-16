#ifndef LOCAL_MESSAGE_REPOSITORY_H
#define LOCAL_MESSAGE_REPOSITORY_H

#include "Database/i_local_database.h"
#include "Repositories/i_local_message_repository.h"

class LocalMessageRepository : public ILocalMessageRepository {
public:
  explicit LocalMessageRepository(ILocalDatabase* dbObs);

  std::vector<CachedMessage> FindByChatId(std::string_view chatId, std::size_t limit, std::size_t offset) override;

  std::vector<CachedMessage> Search(std::string_view chatId, std::string_view keywords) override;

  void Delete(std::string_view messageId) override;

private:
  ILocalDatabase* db_obs_;
};

inline LocalMessageRepository::LocalMessageRepository(ILocalDatabase* dbObs) : db_obs_(dbObs) {}


#endif // LOCAL_MESSAGE_REPOSITORY_H