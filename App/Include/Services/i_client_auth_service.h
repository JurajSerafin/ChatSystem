#ifndef I_CLIENT_AUTH_SERVICE_H
#define I_CLIENT_AUTH_SERVICE_H

#include "Database/local_db_models.h"
#include "User/user.h"

#include <string_view>

/**
 * @brief Service orchestrating user authentication and local session management.
 * * Coordinates network requests, cryptographic key generation/unlocking,
 * and local SQLite database updates during the login and registration flows.
 */
class IClientAuthService {
public:
  virtual ~IClientAuthService() = default;

  /**
   * @brief Registers a new user, generates their cryptographic key pair, and establishes a session.
   * @param login The desired username.
   * @param password The plaintext password (used to protect the locally generated private key).
   * @return A CachedUser profile representing the newly registered account.
   */
  virtual CachedUser Register(std::string_view login, std::string_view password) = 0;

  /**
   * @brief Authenticates an existing user, unlocks their private key, and stores the session token.
   * @param login The username.
   * @param password The plaintext password (used to authenticate and unlock the local private key).
   * @return A CachedUser profile representing the logged-in account.
   */
  virtual CachedUser Login(std::string_view login, std::string_view password) = 0;

  /**
   * @brief Terminates the active session and safely cleanses sensitive data.
   * * Notifies the server of the logout, drops the session token, and securely wipes
   * the active private key from memory.
   */
  virtual void Logout() = 0;
};

#endif // I_CLIENT_AUTH_SERVICE_H