#ifndef CORE_API_USER_CONTRACT_H
#define CORE_API_USER_CONTRACT_H

#include <string_view>

namespace api::user {

  namespace routes {
    constexpr std::string_view kSearch = "/users/search";
    constexpr std::string_view kGetById = "/users/{id}";
    constexpr std::string_view kGetByTag = "/users/tag/{tag}";
    constexpr std::string_view kGetPublicKey = "/users/{id}/public-key";
  }

  namespace path_params {
    constexpr std::string_view kId = "id";
    constexpr std::string_view kTag = "tag";
  }

  namespace query_params {
    constexpr std::string_view kQuery = "q";
  }

  // JSON Payload & Response Fields
  namespace fields {
    constexpr std::string_view kId = "id";
    constexpr std::string_view kTag = "tag";
    constexpr std::string_view kLogin = "login";
    constexpr std::string_view kRole = "role";
    constexpr std::string_view kPublicKey = "public_key";
  }

} // namespace Api::User

#endif // CORE_API_USER_CONTRACT_H