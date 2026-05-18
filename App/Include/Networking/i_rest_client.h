#ifndef I_REST_CLIENT_H
#define I_REST_CLIENT_H

#include "http_response.h"

#include <nlohmann/json.hpp>
#include <string_view>

/**
 * @brief Interface for a REST client used to communicate with the remote server.
 * * Abstracts away the underlying HTTP library (e.g., Boost.Beast, cURL) and
 * natively handles JSON payload serialization/deserialization.
 */
class IRestClient {
public:
  virtual ~IRestClient() = default;

  /**
   * @brief Executes an HTTP GET request to the specified endpoint.
   * @param endpoint The target URL path.
   * @return The HTTP response from the server.
   */
  virtual HttpResponse Get(std::string_view endpoint) = 0;

  /**
   * @brief Executes an HTTP POST request without a body payload.
   * @param endpoint The target URL path.
   * @return The HTTP response from the server.
   */
  virtual HttpResponse Post(std::string_view endpoint) = 0;

  /**
   * @brief Executes an HTTP PUT request without a body payload.
   * @param endpoint The target URL path.
   * @return The HTTP response from the server.
   */
  virtual HttpResponse Put(std::string_view endpoint) = 0;

  /**
   * @brief Executes an HTTP DELETE request to the specified endpoint.
   * @param endpoint The target URL path.
   * @return The HTTP response from the server.
   */
  virtual HttpResponse Delete(std::string_view endpoint) = 0;

  /**
   * @brief Executes an HTTP POST request with a JSON body payload.
   * @param endpoint The target URL path.
   * @param body The JSON payload to send in the request.
   * @return The HTTP response from the server.
   */
  virtual HttpResponse Post(std::string_view endpoint, const nlohmann::json& body) = 0;

  /**
   * @brief Executes an HTTP PUT request with a JSON body payload.
   * @param endpoint The target URL path.
   * @param body The JSON payload to send in the request.
   * @return The HTTP response from the server.
   */
  virtual HttpResponse Put(std::string_view endpoint, const nlohmann::json& body) = 0;
};

#endif // I_REST_CLIENT_H