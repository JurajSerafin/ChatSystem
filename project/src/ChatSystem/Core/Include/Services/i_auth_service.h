#ifndef I_AUTH_SERVICE_H
#define I_AUTH_SERVICE_H

#include <Session/session.h>
#include <User/user.h>
#include <User/user_id.h>
#include <optional>
#include <string>

/**
 * @brief Interface for authentication and session management services.
 *
 * Defines the contract for handling the full authentication lifecycle,
 * including user registration, login/logout, session validation, and
 * password management.
 *
 * Implementations are expected to:
 *  - Securely handle password storage (e.g., hashing).
 *  - Ensure uniqueness of user identifiers (e.g., login).
 *  - Manage session lifecycle, including creation, expiration,
 *  and invalidation.
 *  - Validate input data.
 *
 * @see User
 * @see Session
 */
class IAuthService {
public:
  /**
   * @brief Registers a new user and automatically logs them in.
   *
   * Creates a new user account with the provided login, password, and
   * E2EE public key. Upon successful creation, immediately generates
   * and returns a new active Session.
   * * Implementations should ensure that:
   * - The login is unique.
   * - The password is securely hashed before storage.
   * - The client-generated public key is persisted.
   * - Any required validation rules are applied.
   *
   * @param login The desired unique login name.
   * @param password The plaintext password.
   * @param publicKey The client-generated RSA public key in PEM format.
   * @return A newly created Session containing the access token.
   *
   * @throws std::invalid_argument If the login is already in use
   * or validation fails.
   */
  virtual Session RegisterUser(const std::string& login, const std::string& password, const std::string& publicKey) = 0;

  /**
   * @brief Authenticates a user and creates a session.
   *
   * Verifies the provided credentials and, if valid, creates a new
   * session associated with the user. The returned session typically
   * contains an access token used for subsequent authentication.
   *
   * @param login The user's login name.
   * @param password The plaintext password.
   * @return A valid Session containing an authentication token.
   *
   * @throws std::invalid_argument If authentication fails due to
   * invalid credentials.
   */
  virtual Session Login(const std::string& login, const std::string& password) = 0;


  /**
   * @brief Logs out a user by invalidating their session.
   *
   * Invalidates the session associated with the given token. After this
   * call, the token may no longer be accepted as valid.
   *
   * @param token The session token to invalidate.
   *
   * @note Implementations should treat this operation as idempotent.
   * Invalid or already invalidated tokens should not cause failure.
   */
  virtual void Logout(const std::string& token) = 0;

  /**
   * @brief Validates a session token.
   *
   * Checks whether the provided token is valid (typically, it should exist and not be expired) and
   * returns the associated user if successful.
   *
   * @param token The session token to validate.
   * @return std::optional<User> The associated user if the token is valid;
   * std::nullopt otherwise.
   *
   */
  virtual std::optional<User> ValidateToken(const std::string& token) = 0;

  /**
   * @brief Changes a user's password.
   *
   * Verifies the user's current password and updates it to a new one.
   * Implementations should:
   *  - Validate the old password before allowing the change.
   *  - Securely hash the new password before storing it.
   *
   * @param userId The identifier of the user.
   * @param oldPassword The current plaintext password.
   * @param newPassword The new plaintext password.
   *
   * @throws std::invalid_argument If the user does not exist or
   * the old password is incorrect.
   */
  virtual void ChangePassword(UserId userId,
  const std::string& oldPassword,
  const std::string& newPassword) = 0;

  /**
   * @brief Virtual destructor.
   *
   * Ensures proper cleanup of derived implementations.
   */
  virtual ~IAuthService() = default;
};

#endif  // I_AUTH_SERVICE_H