#ifndef I_REST_CLIENT_H
#define I_REST_CLIENT_H

#include "http_response.h"

#include <nlohmann/json.hpp>
#include <string_view>

class IRestClient {
public:
  virtual HttpResponse Get(std::string_view endpoint) = 0;

  virtual HttpResponse Post(std::string_view endpoint) = 0;

  virtual HttpResponse Put(std::string_view endpoint) = 0;

  virtual HttpResponse Delete(std::string_view endpoint) = 0;

  virtual ~IRestClient() = default;

};

#endif // I_REST_CLIENT_H
