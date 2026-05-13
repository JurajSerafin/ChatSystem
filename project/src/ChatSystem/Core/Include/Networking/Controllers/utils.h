#ifndef NETWORKING_UTILS_H
#define NETWORKING_UTILS_H

#include "Networking/router.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>

#include <boost/beast/http.hpp>
namespace http = boost::beast::http;

/**
 * @namespace netw::utils
 * @brief Stateless helper functions for HTTP request parsing and response generation.
 * 
 * Contains pure functions strictly dedicated to manipulating HTTP strings, headers,
 * URLs, and Boost.Beast response objects. It operates independent of
 * application business logic.
 */
namespace netw::utils {

  /// @brief Standard MIME type for JSON payloads.
  constexpr std::string_view kJsonStrMime = "application/json";

  /// @brief Prefix required for the HTTP Authorization header.
  constexpr std::string_view kAuthSchemeIdf = "Bearer ";

  /// @brief The length of the "Bearer " prefix (7 characters) used for substring parsing.
  constexpr std::size_t kAuthSchemeIdfPrefixLen = 7;

  /// @brief Standard URL query parameter key for pagination limits.
  constexpr std::string_view kLimitField = "limit";

  /// @brief Standard URL query parameter key for pagination offsets.
  constexpr std::string_view kOffsetField = "offset";


  /**
   * @brief Safely extracts the Bearer token from the Authorization header.
   * @return The raw token string, or std::nullopt if missing/invalid.
   */
  std::optional<std::string> ExtractToken(const http::request<http::string_body>& req);


  /**
   * @brief Parses the URL target to extract 'limit' and 'offset' query parameters.
   */
  void ExtractPaginationLimitAndOffset(
    const http::request<http::string_body>& req,
    std::size_t& limitOut,
    std::size_t& offsetOut);

  /**
   * @brief Builds a standard 200 OK response with a JSON payload.
   */
  http::response<http::string_body> BuildAndReturnOkResponse(
    const http::request<http::string_body>& req,
    const nlohmann::json& responseJson);

  /**
   * @brief Builds a standard 204 No Content response for successful PUT/DELETE operations.
   */
  http::response<http::string_body> BuildAndReturnNoContentResponse(const http::request<http::string_body>& req);

  /**
   * @brief Extracts the raw Authorization header from the request.
   * @return The header string, or an empty string if the header is completely missing.
   */
  std::string GetAuthHeader(const http::request<http::string_body>& req);

  /**
   * @brief Checks if the Authorization header contains a properly formatted Bearer token.
   */
  bool IsValidToken(std::string_view authHeader);

  /// @brief Strips the "Bearer " prefix to return the raw token.
  std::string GetToken(std::string_view authHeader);

  /**
   * @brief Safely extracts a path parameter by its key from the router's parsed parameters.
   * @throws std::invalid_argument if the key is missing.
   */
  std::string ExtractPathParam(const Router::PathParams& params, std::string_view key);

} // namespace netw::utils

#endif // NETWORKING_UTILS_H