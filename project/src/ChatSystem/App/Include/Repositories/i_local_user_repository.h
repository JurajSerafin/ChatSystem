#ifndef I_LOCAL_USER_REPOSITORY_H
#define I_LOCAL_USER_REPOSITORY_H

#include "Database/local_db_models.h"

#include <optional>


class ILocalUserRepository {
public:
  virtual ~ILocalUserRepository() = default;

  virtual std::optional<CachedUser> FindById(std::string_view userId) = 0;

  virtual std::optional<CachedUser> FindByLoginOrTag(std::string_view login, std::string_view tag) = 0;

  virtual void Upsert(const CachedUser& user) = 0;

  virtual void Delete(std::string_view userId) = 0;
};

#endif // I_LOCAL_USER_REPOSITORY_H