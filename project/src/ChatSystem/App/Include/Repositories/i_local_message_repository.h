#ifndef I_LOCAL_MESSAGE_REPOSITORY_H
#define I_LOCAL_MESSAGE_REPOSITORY_H

#include "Database/local_db_models.h"

class ILocalMessageRepository {
public:
  virtual ~ILocalMessageRepository() = default;

  virtual std::vector<CachedMessage> FindByChatId(std::string_view chatId, std::size_t limit, std::size_t offset) = 0;

  virtual std::vector<CachedMessage> Search(std::string_view chatId, std::string_view keywords) = 0;

  virtual void Upsert(const CachedMessage& message) = 0;

  virtual void Delete(std::string_view messageId) = 0;
};

#endif // I_LOCAL_MESSAGE_REPOSITORY_H