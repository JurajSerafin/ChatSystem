#ifndef CORE_API_AUTH_CONTRACT_H
#define CORE_API_AUTH_CONTRACT_H

#include <string_view>

namespace api::auth {
  // Routes

  namespace routes {
    constexpr std::string_view kRegisterRoute = "/auth/register";
    constexpr std::string_view kLoginRoute = "/auth/login";
    constexpr std::string_view kLogoutRoute = "/auth/logout";
    constexpr std::string_view kChangePasswordRoute = "/auth/password";
  } // namespace api::auth::routes

  // JSON Payload and Payload Fields
  namespace fields {
    constexpr std::string_view kLogin = "login";
    constexpr std::string_view kPassword = "password";
    constexpr std::string_view kPublicKey = "public_key";
    constexpr std::string_view kOldPassword = "old_password";
    constexpr std::string_view kNewPassword = "new_password";
    constexpr std::string_view kToken = "token";
  }// namespace api::auth::fields

} // namespace Api::Auth

#endif // CORE_API_AUTH_CONTRACT_H