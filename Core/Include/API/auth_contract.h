#ifndef CORE_API_AUTH_CONTRACT_H
#define CORE_API_AUTH_CONTRACT_H

#include <string_view>

/**
 * @brief API contract definitions for authentication endpoints.
 * Contains URL routes and JSON field identifiers for user registration, login, and session management.
 */
namespace api::auth {

  /** @brief HTTP routing paths for authentication operations. */
  namespace routes {
    /// POST: Registers a new user with a password and public key.
    constexpr std::string_view kRegisterRoute = "/auth/register";

    /// POST: Authenticates a user and issues a session token.
    constexpr std::string_view kLoginRoute = "/auth/login";

    /// POST: Terminates an active session and invalidates the token.
    constexpr std::string_view kLogoutRoute = "/auth/logout";

    /// PUT: Updates a user's password.
    constexpr std::string_view kChangePasswordRoute = "/auth/password";

  } // namespace api::auth::routes

  /** @brief JSON payload keys used in authentication requests and responses. */
  namespace fields {
    constexpr std::string_view kLogin = "login";

    constexpr std::string_view kPassword = "password";

    constexpr std::string_view kPublicKey = "public_key";

    constexpr std::string_view kOldPassword = "old_password";

    constexpr std::string_view kNewPassword = "new_password";

    constexpr std::string_view kToken = "token";

  } // namespace api::auth::fields

} // namespace api::auth

#endif // CORE_API_AUTH_CONTRACT_H