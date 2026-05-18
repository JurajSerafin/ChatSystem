#ifndef LOCAL_USER_REPOSITORY_H
#define LOCAL_USER_REPOSITORY_H

#include "Database/i_local_database.h"
#include "Repositories/i_local_user_repository.h"

/**
 * @brief Concrete implementation of the local user repository.
 * * Handles caching and retrieving public user profiles, ensuring cryptographic
 * public keys are accessible on a short notice, without making redundant network calls.
 */
class LocalUserRepository : public ILocalUserRepository {
public:
  /**
   * @brief Constructs the repository with an injected database connection.
   * @param dbObs Pointer to the active local database instance.
   */
  explicit LocalUserRepository(ILocalDatabase* dbObs);

  std::optional<CachedUser> FindById(std::string_view userId) override;

  std::optional<CachedUser> FindByLoginOrTag(std::string_view login, std::string_view tag) override;

  void Upsert(const CachedUser& user) override;

  void Delete(std::string_view userId) override;

private:
  ILocalDatabase* db_obs_;
};

inline LocalUserRepository::LocalUserRepository(ILocalDatabase* dbObs) : db_obs_(dbObs) {}

#endif // LOCAL_USER_REPOSITORY_H