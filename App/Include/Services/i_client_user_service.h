#ifndef I_CLIENT_USER_SERVICE_H
#define I_CLIENT_USER_SERVICE_H

#include "Database/local_db_models.h"
#include "User/user_id.h"

#include <string_view>
#include <vector>

/**
 * @brief Service responsible for querying and caching public user profiles.
 * * Orchestrates network lookups and SQLite caching to ensure the client
 * always has access to the public keys required for message encryption.
 */
class IClientUserService {
public:
  virtual ~IClientUserService() = default;

  /**
   * @brief Searches the global user directory on the server for matching accounts.
   * @param query The search string (can be a login or tag).
   * @param limit The maximum number of results to return.
   * @param offset The pagination offset.
   * @return A vector of matching user profiles.
   */
  virtual std::vector<CachedUser> Search(std::string_view query, std::size_t limit, std::size_t offset) = 0;

  /**
   * @brief Retrieves a specific user profile.
   * * First checks the local cache. If missing, it fetches the profile from
   * the server and persists it locally.
   * @param userId The unique identifier of the user.
   * @return The requested CachedUser profile.
   */
  virtual CachedUser GetById(const UserId& userId) = 0;

  /**
   * @brief Retrieves the public key of a specific user.
   * * Crucial for E2EE wrapping. Automatically fetches and caches the entire
   * user profile if they do not currently exist in the local database.
   * @param userId The unique identifier of the user.
   * @return The user's public key string.
   */
  virtual std::string GetPublicKey(const UserId& userId) = 0;
};

#endif // I_CLIENT_USER_SERVICE_H