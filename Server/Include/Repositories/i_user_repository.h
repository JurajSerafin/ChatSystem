#ifndef I_USER_REPOSITORY_H
#define I_USER_REPOSITORY_H

#include <optional>
#include <string>
#include <vector>

#include <User/user_id.h>
#include <User/user.h>

/**
 * @brief Server-side repository interface for managing global user accounts.
 * * Handles the persistence of user profiles, login credentials, and the
 * distribution of cryptographic public keys for E2EE wrapping.
 */
class IUserRepository {
public:
  virtual ~IUserRepository() = default;

  /**
   * @brief Retrieves a specific user by their unique database identifier.
   * @param id The unique identifier of the user.
   * @return An optional containing the user if found.
   */
  virtual std::optional<User> FindById(const UserId& id) = 0;

  /**
   * @brief Retrieves a user by their exact login username.
   * * Primarily used during the authentication phase to validate login attempts.
   * @param login The login username.
   * @return An optional containing the user if found.
   */
  virtual std::optional<User> FindByLogin(const std::string& login) = 0;

  /**
   * @brief Retrieves a user by their exact, globally unique display tag.
   * @param tag The globally unique tag struct.
   * @return An optional containing the user if found.
   */
  virtual std::optional<User> FindByTag(const tags::UserTag& tag) = 0;

  /**
   * @brief Performs a paginated search for users matching a specific query string.
   * * Can search across partial usernames or tags to populate user discovery UI.
   * @param query The partial string to search for.
   * @param limit The maximum number of results to return.
   * @param offset The pagination offset.
   * @return A vector of matching users.
   */
  virtual std::vector<User> Search(const std::string& query, std::size_t limit, std::size_t offset) = 0;

  /**
   * @brief Adds a newly registered user account to the database.
   * @param user The new user entity to save.
   */
  virtual void Add(const User& user) = 0;

  /**
   * @brief Updates an existing user's mutable profile details (e.g., password hash, roles).
   * @param user The user entity containing the updated state.
   */
  virtual void Update(const User& user) = 0;

  /**
   * @brief Permanently deletes a user account from the system.
   * @param id The ID of the user to delete.
   */
  virtual void DeleteById(const UserId& id) = 0;
};

#endif // I_USER_REPOSITORY_H