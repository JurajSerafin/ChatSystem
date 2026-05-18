#ifndef I_LOCAL_USER_REPOSITORY_H
#define I_LOCAL_USER_REPOSITORY_H

#include "Database/local_db_models.h"

#include <optional>

/**
 * @brief Repository interface for managing cached user profiles.
 * * Handles the persistence of public user profiles and their public keys,
 * allowing the client to look up participants without hitting the network.
 */
class ILocalUserRepository {
public:
  virtual ~ILocalUserRepository() = default;

  /**
   * @brief Retrieves a specific user profile by their unique ID.
   * @param userId The ID of the user to locate.
   * @return An optional containing the user profile if found locally, std::nullopt otherwise.
   */
  virtual std::optional<CachedUser> FindById(std::string_view userId) = 0;

  /**
   * @brief Retrieves a user profile by either an exact login username or a display tag.
   * @param login The login username to search for.
   * @param tag The tag/handle to search for.
   * @return An optional containing the matching user profile, std::nullopt otherwise.
   */
  virtual std::optional<CachedUser> FindByLoginOrTag(std::string_view login, std::string_view tag) = 0;

  /**
   * @brief Inserts a new user profile or updates an existing one in the local cache.
   * @param user The user profile object to save.
   */
  virtual void Upsert(const CachedUser& user) = 0;

  /**
   * @brief Removes a user profile from the local cache.
   * @param userId The ID of the user to delete.
   */
  virtual void Delete(std::string_view userId) = 0;
};

#endif // I_LOCAL_USER_REPOSITORY_H