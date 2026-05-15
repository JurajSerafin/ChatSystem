#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include <Services/Interface/i_user_service.h>
#include <Repositories/i_user_repository.h>
#include <User/user.h>

/**
 * @brief Handles read-only operations for user discovery and profile retrieval.
 * 
 * This service deliberately restricts access to the core User entity (which contains
 * sensitive data like password hashes) and maps database records to safe UserProfile
 * objects for client consumption.
 */
class UserService : public IUserService {
public:
  explicit UserService(IUserRepository* userRepoObs);

  /**
   * @brief Performs a paginated search for users by their login or tag.
   * 
   * @param query The search string (supports partial matches).
   * @param requestingUserId The user performing the search.
   * @param limit Maximum number of profiles to return.
   * @param offset Pagination offset.
   * @return A vector of safe UserProfile objects matching the query.
   */
  [[nodiscard]] std::vector<UserProfile> Search(
    const std::string& query,
    const UserId& requestingUserId,
    std::size_t limit,
    std::size_t offset
  ) override;

  /**
   * @brief Retrieves a specific user's public profile by their unique ID.
   *
   * @param userId The target user's UUID.
   * @return The UserProfile, or std::nullopt if the user does not exist.
   */
  [[nodiscard]] std::optional<UserProfile> GetById(const UserId& userId) override;

  /**
   * @brief Retrieves a specific user's public profile by their unique UserTag.
   *
   * @param tag The human-readable tag (e.g., User#1234).
   * @return The UserProfile, or std::nullopt if the user does not exist.
   */
  [[nodiscard]] std::optional<UserProfile> GetByTag(const tags::UserTag& tag) override;

  /**
   * @brief Retrieves the raw RSA public key string for a user.
   *
   * This is a critical endpoint for the E2EE flow, allowing clients to encrypt
   * AES session keys before dispatching messages.
   *
   * @param userId The target user's UUID.
   * @return The Base64 encoded RSA public key, or std::nullopt if not found.
   */
  [[nodiscard]] std::optional<std::string> GetPublicKey(const UserId& userId) override;

private:
  IUserRepository* user_repo_obs_;

  /// @brief Maps an internal User domain entity to a UserProfile DTO for easy transport and serialization. 
  static UserProfile MapToProfile(const User& user);
};

#endif // USER_SERVICE_H