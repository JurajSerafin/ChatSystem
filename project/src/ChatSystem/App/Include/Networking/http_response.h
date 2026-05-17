#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "nlohmann/json.hpp"

struct HttpResponse {
  unsigned status_code;
  nlohmann::json body;
};

#endif // HTTP_RESPONSE_H