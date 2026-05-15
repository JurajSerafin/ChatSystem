#ifndef ROUTER_H
#define ROUTER_H

#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

/**
 * @brief HTTP router.
 *
 * Maps incoming HTTP requests to specific controller handlers based on the HTTP verb
 * and the URI path.
 * * It pre-computes route templates at startup to ensure the routing "hot path"
 * performs no dynamic memory allocations for templates.
 */
class Router {
public:
  /**
   * @brief A container for extracted URI variables.
   * Uses std::string to guarantee memory ownership and prevent dangling pointers.
   */
  using PathParams = std::vector<std::pair<std::string, std::string>>;

  /// @brief The signature for controller methods handling matched routes.
  using HandlerFunc = std::function<http::response<http::string_body>(
    const http::request<http::string_body>&,
    const PathParams& params
  )>;

  /// @brief Represents a pre-compiled REST endpoint.
  struct Route {
    http::verb method;
    std::vector<std::string> segments;     ///< Static segments
    std::vector<std::string> param_names;  ///< Names of dynamic segments
    std::vector<uint8_t> is_dynamic;       ///< Fast lookup flags for dynamic segments
    HandlerFunc handler;                   ///< The controller callback
  };

  /**
   * @brief Registers a new REST endpoint.
   */
  void AddRoute(http::verb method, const std::string& pathTemplate, HandlerFunc handler);

  /**
   * @brief The routing hot-path. Matches an incoming request to a registered route.
   */
  http::response<http::string_body> RouteRequest(const http::request<http::string_body>& req);

private:

  /// @brief Checks if a template segment is a variable bounded by {braces}.
  static bool IsDynamicParameter(std::string_view segment);

  /// @brief Extracts the variable name and flags the segment as dynamic.
  static void ProcessDynamicParameter(Route& route, std::string_view param);

  /// @brief Registers an exact string match requirement for a segment.
  static void ProcessStringMatch(Route& route, std::string&& param);

  /// @brief Parses a complete URI template into the internal Route structure.
  static void ProcessRouteTemplate(const std::string& pathTemplate, Route& route);

  /**
   * @brief Attempts to match request segments against a route's template segments.
   */
  static bool TryExtractParams(
    PathParams& paramsOut,
    const std::vector<uint8_t>& isDynamicIndicators,
    const std::vector<std::string>& paramNames,
    const std::vector<std::string>& requestSegments, // FIXED: Now strictly a vector of strings
    const std::vector<std::string>& segments
  );

  /// @brief Generates a standard 404 Not Found response.
  static http::response<http::string_body> HandleNotFound(const http::request<http::string_body>& req);

  /// @brief Generates a standard 400 Bad Request response for malformed URIs.
  static http::response<http::string_body> HandleBadRequest(const http::request<http::string_body>& req);

  static std::string ExtractTarget(const http::request<http::string_body>& req);

  static std::string ExtractPath(const std::string& target);

  static void DiscardLeadingEmptyString(const std::string& path, std::istream& pathStream, std::string& segment);

  static std::vector<std::string> GetParsedRequestSegments(const std::string& path, std::istream& pathStream, std::string& segment);
  
  std::vector<Route> routes_;
};

#endif // ROUTER_H