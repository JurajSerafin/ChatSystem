#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "nlohmann/json.hpp"

/**
 * @brief Represents a standard HTTP response returned by the REST client.
 */
struct HttpResponse {
  /** @brief The HTTP status code. */
  unsigned status_code;

  /** @brief The parsed JSON payload returned by the server. */
  nlohmann::json body;
};

#endif // HTTP_RESPONSE_H