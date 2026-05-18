#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H

#include "Networking/router.h"
#include "Services/i_auth_service.h"
#include "Services/i_user_service.h"

#include <nlohmann/json.hpp>
#include <string_view>

#include <boost/beast/http.hpp>

/**
 * @brief Presentation layer controller for handling Authentication REST API endpoints.
 *
 * Responsible for user registration, session creation (login), session termination (logout),
 * and credential management. It translates raw HTTP requests, structurally validates
 * the JSON payloads, and delegates business logic to the IAuthService.
 */
class AuthController {
public:
  explicit AuthController(IAuthService* authServiceObs, IUserService* userServiceObs);

  /**
   * @brief Registers a new user and provisions a session token.
   * @route POST /auth/register
   * @body { "login": "...", "password": "...", "public_key": "..." }
   */
  http::response<http::string_body> HandleRegister(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  /**
   * @brief Authenticates a user and returns a session token.
   * @route POST /auth/login
   * @body { "login": "...", "password": "..." }
   */
  http::response<http::string_body> HandleLogin(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  /**
   * @brief Terminates the active session associated with the provided Bearer token.
   * @route POST /auth/logout
   * @header Authorization: Bearer <token>
   */
  http::response<http::string_body> HandleLogout(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  /**
   * @brief Updates the authenticated user's password.
   * @route PUT /auth/password
   * @header Authorization: Bearer <token>
   * @body { "old_password": "...", "new_password": "..." }
   */
  http::response<http::string_body> HandleChangePassword(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  void RegisterRoutes(Router& router);

private:

  static std::string ExtractRequiredString(const nlohmann::json& body, std::string_view fieldName);

  static nlohmann::json FormatAuthResponse(const std::string& token, const UserProfile& userProfile);

  static nlohmann::json FormatTokenResponse(const std::string& token);

  std::string RegisterAndGetSessionToken(const nlohmann::json& authDataBody) const;

  std::string LoginAndGetSessionToken(const nlohmann::json& authDataBody) const;

  void ExtractAndChangePassword(const nlohmann::json& passChangeBody, const UserId& userId) const;

  IAuthService* auth_service_obs_;
  IUserService* user_service_obs_;
};

inline AuthController::AuthController(IAuthService* authServiceObs, IUserService* userServiceObs) : auth_service_obs_(authServiceObs), user_service_obs_(userServiceObs) {}

#endif // AUTH_CONTROLLER_H