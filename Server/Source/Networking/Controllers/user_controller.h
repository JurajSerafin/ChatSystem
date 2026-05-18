#ifndef USER_CONTROLLER_H
#define USER_CONTROLLER_H

#include "Networking/router.h"
#include "Services/i_auth_service.h"
#include "Services/i_user_service.h"

#include <nlohmann/json.hpp>
#include <optional>

#include <boost/beast/http.hpp>

/**
 * @brief Presentation layer controller for User discovery and profile management endpoints.
 *
 * Exposes endpoints for searching users by tag or login, fetching specific user profiles
 * by their ID or Tag, and retrieving the public RSA keys required
 * by clients to establish End-to-End Encryption.
 */
class UserController {
public:
  explicit UserController(IAuthService* authServiceObs, IUserService* userServiceObs);

  /**
   * @brief Searches for users by login or tag, filtering out the requesting user.
   * @route GET /users/search?q=&limit=&offset=
   */
  http::response<http::string_body> HandleSearchUsers(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  /**
   * @brief Fetches a user's public profile by their internal immutable ID.
   * @route GET /users/{id}
   */
  http::response<http::string_body> HandleGetById(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  /**
   * @brief Fetches a user's public profile by their unique tag.
   * @route GET /users/tag/{tag}
   */
  http::response<http::string_body> HandleGetByTag(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  /**
   * @brief Retrieves a user's public RSA key used for encrypting messages.
   * @route GET /users/{id}/public-key
   */
  http::response<http::string_body> HandleGetPublicKey(const http::request<http::string_body>& req, const Router::PathParams& params) const;

  void RegisterRoutes(Router& router);

private:
  std::optional<UserId> GetAuthenticatedUserId(const http::request<http::string_body>& req) const;

  static nlohmann::json FormatJsonOutput(const std::vector<UserProfile>& userProfiles);

  static nlohmann::json FormatJsonOutput(const UserProfile& userProfile);

  static std::string ExtractSearchQuery(const http::request<http::string_body>& req);

  IAuthService* auth_service_obs_;
  IUserService* user_service_obs_;
};

#endif // USER_CONTROLLER_H