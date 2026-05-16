#ifndef I_LOCAL_IDENTITY_REPOSITORY_H
#define I_LOCAL_IDENTITY_REPOSITORY_H

#include <string_view>

#include "Database/local_db_models.h"

class ILocalIdentityRepository {
public:
  virtual ~ILocalIdentityRepository() = default;

  virtual void Store(const CachedIdentity& identity) = 0;

  virtual std::optional<CachedIdentity> Load() = 0;

  virtual void UpdateSessionToken(std::string_view newToken) = 0;

  virtual void Clear() = 0;
};

#endif // I_LOCAL_IDENTITY_REPOSITORY_H