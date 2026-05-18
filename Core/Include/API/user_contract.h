#ifndef CORE_API_USER_CONTRACT_H
#define CORE_API_USER_CONTRACT_H

#include <string_view>

/**
 * @brief API contract definitions for user profile queries.
 * Contains URL routes and fields for searching the global user directory and retrieving public keys.
 */
namespace api::user {

  /** @brief HTTP routing paths for user-related queries. */
  namespace routes {
    /// GET a list of users matching a search query.
    constexpr std::string_view kSearch = "/users/search";

    /// GET a specific user's public profile by their ID.
    constexpr std::string_view kGetById = "/users/{id}";

    /// GET a specific user's public profile by their display tag.
    constexpr std::string_view kGetByTag = "/users/tag/{tag}";

    /// GET only the public asymmetric key for a specific user ID.
    constexpr std::string_view kGetPublicKey = "/users/{id}/public-key";

  } // namespace api::user::routes

  /** @brief Template parameters used in URL path substitution. */
  namespace path_params {
    constexpr std::string_view kId = "id";

    constexpr std::string_view kTag = "tag";

  } // namespace api::user::path_params

  /** @brief Query parameters used for search endpoints. */
  namespace query_params {
    /// Query key containing the plaintext search term.
    constexpr std::string_view kQuery = "q";

  } // namespace api::user::query_params

  /** @brief JSON payload keys used in user profile responses. */
  namespace fields {

    constexpr std::string_view kId = "id";

    constexpr std::string_view kTag = "tag";

    constexpr std::string_view kLogin = "login";

    constexpr std::string_view kRole = "role";

    constexpr std::string_view kPublicKey = "public_key";

  } // namespace api::user::fields

} // namespace api::user

#endif // CORE_API_USER_CONTRACT_H