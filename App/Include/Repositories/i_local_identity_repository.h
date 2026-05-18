#ifndef I_LOCAL_IDENTITY_REPOSITORY_H
#define I_LOCAL_IDENTITY_REPOSITORY_H

#include <string_view>

#include "Database/local_db_models.h"

/**
 * @brief Repository interface for securely managing the active user's session identity.
 * * Handles the persistence of the current logged-in user profile and their
 * active network session token.
 */
class ILocalIdentityRepository {
public:
  virtual ~ILocalIdentityRepository() = default;

  /**
   * @brief Saves the active user's identity and session to the local cache.
   * @param identity The identity object to persist.
   */
  virtual void Store(const CachedIdentity& identity) = 0;

  /**
   * @brief Retrieves the currently logged-in user's identity.
   * @return An optional containing the identity if an active session exists, std::nullopt otherwise.
   */
  virtual std::optional<CachedIdentity> Load() = 0;

  /**
   * @brief Refreshes the active session token without modifying the rest of the user's identity.
   * @param newToken The new network session token.
   */
  virtual void UpdateSessionToken(std::string_view newToken) = 0;

  /**
   * @brief Wipes the active identity and session token from the local cache.
   */
  virtual void Clear() = 0;
};

#endif // I_LOCAL_IDENTITY_REPOSITORY_H