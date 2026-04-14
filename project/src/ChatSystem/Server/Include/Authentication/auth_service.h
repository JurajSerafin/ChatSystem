#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include <Repositories/i_session_repository.h>
#include <Repositories/i_user_repository.h>
#include <Services/i_auth_service.h>
#include <Services/i_encryption_service.h>
#include <Session/session.h>
#include <Session/session_id.h>
#include <Session/session_params.h>
#include <Tags/tag.h>
#include <User/user_id.h>
#include <User/user_params.h>
#include <Validation/i_validator.h>
#include <chrono>
#include <string>

/**
 * @brief Concrete implementation of IAuthService.
 *
 * Handles the full authentication lifecycle: registration, login,
 * logout, token validation, and password changes.
 *
 * @see IAuthService
 * @see IUserRepository
 * @see ISessionRepository
 * @see IEncryptionService
 */
class AuthService : public IAuthService {
public:
  /**
   * @brief Constructs the AuthService with all required dependencies.
   *
   * @param userRepo Repository for user persistence and lookup.
   * @param sessionRepo Repository for session persistence and lookup.
   * @param encryptionService Service for hashing, key generation, and verification.
   * @param userValidator Validator applied during user registration.
   * @param sessionValidator Validator applied during session creation.
   */
  AuthService(IUserRepository& userRepo,
    ISessionRepository& sessionRepo,
    IEncryptionService& encryptionService,
    const IValidator<UserParams>& userValidator,
    const IValidator<SessionParams>& sessionValidator)
    : user_repo_{userRepo}
    , session_repo_{sessionRepo}
    , encryption_service_{encryptionService}
    , user_validator_{userValidator}
    , session_validator_{sessionValidator} {}

  AuthService(const AuthService&) = delete;

  AuthService& operator=(const AuthService&) = delete;

  AuthService(AuthService&&) = delete;

  AuthService& operator=(AuthService&&) = delete;

  /**
   * @brief Registers a new user with the given login and password.
   *
   * Checks for login uniqueness, generates a unique tag, hashes the
   * password, generates a key pair, validates, and persists the user.
   *
   * @param login The desired login name.
   * @param password The plaintext password to hash and store.
   * @return The newly created and persisted User.
   * @throws std::invalid_argument if the login is already taken or validation fails.
   */
  User RegisterUser(const std::string& login, const std::string& password) override;

  /**
   * @brief Authenticates a user and creates a new session.
   *
   * @param login The user's login name.
   * @param password The plaintext password to verify.
   * @return A newly created Session containing the access token.
   * @throws std::invalid_argument if credentials are invalid.
   */
  Session Login(const std::string& login, const std::string& password) override;

  /**
   * @brief Invalidates the session associated with the given token.
   *
   * @param token The session token to invalidate.
   */
  void Logout(const std::string& token) override;

  /**
   * @brief Validates a session token and returns the associated user.
   *
   * @param token The session token to validate.
   * @return The User associated with the token, or std::nullopt if the
   * token is invalid or expired.
   */
  std::optional<User> ValidateToken(const std::string& token) override;

  /**
   * @brief Changes the password for the given user.
   *
   * @param userId The ID of the user changing their password.
   * @param oldPassword The current plaintext password for verification.
   * @param newPassword The new plaintext password to hash and store.
   * @throws std::invalid_argument if the user is not found or the
   * current password is incorrect.
   */
  void ChangePassword(UserId userId, const std::string& oldPassword, const std::string& newPassword) override;

private:
  /**
   * @brief Generates a tag unique among all existing users.
   *
   * Retries up to kMaxTagRetries times on collision.
   *
   * @param login The login used as the tag base.
   * @return A unique Tag for the new user.
   * @throws std::runtime_error if a unique tag cannot be generated
   * within the retry limit.
   */
  Tag GenerateUniqueTag(const std::string& login) const;

  /**
   * @brief Generates a cryptographically random session token.
   *
   * @return A UUID string representing a session token.
   */
  static std::string GenerateToken();

  /// Duration after which a session expires.
  static constexpr auto kSessionDuration = std::chrono::hours{ 24 };

 /**
  * Reference fields are intentional here because:
  *  - All dependencies are required and never null
  *  - All dependencies outlive this service — they are constructed before
  *    it and destroyed after it in server_main.cpp → no dangling risk.
  *  - This class is non-copyable and non-movable by design
  */
  IUserRepository& user_repo_;

  ISessionRepository& session_repo_;

  IEncryptionService& encryption_service_;

  const IValidator<UserParams>& user_validator_;

  const IValidator<SessionParams>& session_validator_;
};

inline void AuthService::Logout(const std::string& token) {
  session_repo_.DeleteByToken(token);
}

inline std::string AuthService::GenerateToken() {
  return SessionId::Generate().ToString();
}

#endif  // AUTH_SERVICE_H