#ifndef LOCAL_IDENTITY_REPOSITORY_H
#define LOCAL_IDENTITY_REPOSITORY_H

#include "Database/i_local_database.h"
#include "Database/local_db_models.h"
#include "Repositories/i_local_identity_repository.h"

#include <optional>

class LocalIdentityRepository : public ILocalIdentityRepository {
public:
  explicit LocalIdentityRepository(ILocalDatabase* dbObs);

  void Store(const CachedIdentity& identity) override;

  std::optional<CachedIdentity> Load() override;

  void UpdateSessionToken(std::string_view newToken) override;

  void Clear() override;

private:
  ILocalDatabase* db_obs_;
};

inline LocalIdentityRepository::LocalIdentityRepository(ILocalDatabase* dbObs) : db_obs_(dbObs) {}

#endif // LOCAL_IDENTITY_REPOSITORY_H