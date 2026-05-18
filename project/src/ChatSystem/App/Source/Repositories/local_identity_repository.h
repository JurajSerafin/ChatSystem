#ifndef LOCAL_IDENTITY_REPOSITORY_H
#define LOCAL_IDENTITY_REPOSITORY_H

#include "Database/i_local_database.h"
#include "Database/local_db_models.h"
#include "Repositories/i_local_identity_repository.h"

#include <optional>

/**
 * @brief Concrete implementation of the local identity repository.
 * * Manages the persistence of the active user session and authentication token
 * by delegating to the low-level database engine.
 */
class LocalIdentityRepository : public ILocalIdentityRepository {
public:
  /**
   * @brief Constructs the repository with an injected database connection.
   * @param dbObs Pointer to the active local database instance.
   */
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