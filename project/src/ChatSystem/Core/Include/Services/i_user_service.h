#ifndef I_USER_SERVICE_H
#define I_USER_SERVICE_H

#include "DTOs/user_profile.h"

#include <User/user_id.h>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Interface for user discovery and public key distribution.
 *
 * Handles searching for users via plaintext metadata (Login/Tag) and distributing
 * the public keys required for clients to perform E2EE key wrapping.
 */
class IUserService {
public:
  virtual ~IUserService() = default;

  /**
   * @brief Searches for users by matching a query against their Login or Tag.
   *
   * @param query The partial or full string to search for.
   * @param requestingUserId The ID of the user performing the search.
   * @param limit The maximum number of results.
   * @param offset The pagination offset.
   * @return A vector of UserProfile DTOs matching the search criteria.
   */
  [[nodiscard]] virtual std::vector<UserProfile> Search(
    const std::string& query,
    const UserId& requestingUserId,
    std::size_t limit,
    std::size_t offset
  ) = 0;

  /**
   * @brief Retrieves a specific user's public profile by their ID.
   *
   * @param userId The target user ID.
   * @return The UserProfile DTO, or std::nullopt if not found.
   */
  [[nodiscard]] virtual std::optional<UserProfile> GetById(const UserId& userId) = 0;

  /**
   * @brief Retrieves a specific user's public profile by their unique Tag.
   *
   * @param tag The exact UserTag to look up.
   * @return The UserProfile DTO, or std::nullopt if not found.
   */
  [[nodiscard]] virtual std::optional<UserProfile> GetByTag(const tags::UserTag& tag) = 0;

  /**
   * @brief Retrieves the public key for a specific user.
   *
   * Necessary for the E2EE pipeline. Clients call this before sending a message
   * to wrap the symmetric key for the recipient.
   *
   * @param userId The ID of the target user.
   * @return The public key in PEM format, or std::nullopt if the user doesn't exist.
   */
  [[nodiscard]] virtual std::optional<std::string> GetPublicKey(const UserId& userId) = 0;
};

#endif // I_USER_SERVICE_H