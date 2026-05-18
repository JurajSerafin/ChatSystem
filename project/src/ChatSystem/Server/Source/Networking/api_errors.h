#ifndef API_ERRORS_H
#define API_ERRORS_H

#include <string_view>
#include <boost/beast/http.hpp>
#include <nlohmann/json.hpp>

namespace http = boost::beast::http;

/**
 * @brief Standardized JSON error response generators for Controllers.
 */
namespace api::errors {

  /**
   * @brief The core engine for building an error response.
   * Creates a JSON payload of {"error": "..."} and prepares headers.
   */
  inline http::response<http::string_body> BuildErrorResponse(
    const http::request<http::string_body>& req,
    http::status status,
    std::string_view message)
  {
    http::response<http::string_body> res{ status, req.version() };

    res.set(http::field::content_type, "application/json");

    nlohmann::json error_body = {
        {"error", message}
    };

    res.body() = error_body.dump();

    res.prepare_payload();

    return res;
  }

  /** @brief 400 Bad Request: Malformed JSON, missing fields, or invalid UUIDs. */
  inline http::response<http::string_body> BadRequest(
    const http::request<http::string_body>& req,
    const std::string_view message = "Bad Request. Invalid syntax or missing parameters.")
  {
    return BuildErrorResponse(req, http::status::bad_request, message);
  }

  /** @brief 401 Unauthorized: Missing, invalid, or expired Bearer token. */
  inline http::response<http::string_body> Unauthorized(
    const http::request<http::string_body>& req,
    const std::string_view message = "Unauthorized. Invalid or missing authentication token.")
  {
    return BuildErrorResponse(req, http::status::unauthorized, message);
  }

  /** @brief 403 Forbidden: Valid token, but user lacks permission. */
  inline http::response<http::string_body> Forbidden(
    const http::request<http::string_body>& req,
    std::string_view message = "Forbidden. You do not have permission to access this resource.")
  {
    return BuildErrorResponse(req, http::status::forbidden, message);
  }

  /** @brief 404 Not Found: The requested resource (User, Chat, Message) does not exist. */
  inline http::response<http::string_body> NotFound(
    const http::request<http::string_body>& req,
    const std::string_view message = "Resource not found.")
  {
    return BuildErrorResponse(req, http::status::not_found, message);
  }

  /** @brief 409 Conflict: Business logic violation (e.g., trying to register an existing login). */
  inline http::response<http::string_body> Conflict(
    const http::request<http::string_body>& req,
    std::string_view message = "Conflict. The resource already exists or state is invalid.")
  {
    return BuildErrorResponse(req, http::status::conflict, message);
  }

  /** @brief 500 Internal Server Error: Database crashes, null pointers, unhandled exceptions. */
  inline http::response<http::string_body> InternalServerError(
    const http::request<http::string_body>& req,
    std::string_view message = "An unexpected internal server error occurred.")
  {
    return BuildErrorResponse(req, http::status::internal_server_error, message);
  }

} // namespace api::errors

#endif // API_ERRORS_H