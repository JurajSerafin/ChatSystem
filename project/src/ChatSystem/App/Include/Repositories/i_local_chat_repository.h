#ifndef I_LOCAL_CHAT_REPOSITORY_H
#define I_LOCAL_CHAT_REPOSITORY_H

#include <vector>
#include <string_view>

#include "Database/local_db_models.h"

class ILocalChatRepository {
public:
  virtual ~ILocalChatRepository() = default;

  virtual std::vector<CachedChat> FindAll() = 0;

  virtual std::optional<CachedChat> FindById(std::string_view chatId) = 0;

  virtual void Upsert(const CachedChat& chat) = 0;

  virtual void UpdateLastMessage(std::string_view chatId, std::string_view messageId, int64_t lastActivityMs) = 0;

  virtual void Delete(std::string_view chatId) = 0;
};

#endif // I_LOCAL_CHAT_REPOSITORY_H