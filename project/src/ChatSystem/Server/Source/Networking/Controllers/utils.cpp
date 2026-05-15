#include "Networking/Controllers/utils.h"

#include "boost/url/parse.hpp"

namespace netw::utils {
std::optional<std::string> ExtractToken(const http::request<http::string_body>& req) {
  const auto it = req.find(http::field::authorization);

  if (it == req.end()) {
    return std::nullopt;
  }

  const std::string_view auth_header = it->value();

  if (auth_header.empty() || !auth_header.starts_with(kAuthSchemeIdf)) {
    return std::nullopt;
  }

  return std::string(auth_header.substr(kAuthSchemeIdfPrefixLen));
}

void ExtractPaginationLimitAndOffset(
  const http::request<http::string_body>& req,
  std::size_t& limitOut,
  std::size_t& offsetOut)
{
  if (auto rv = boost::urls::parse_origin_form(req.target())) {
    const auto params = rv->params();

    if (const auto it = params.find(kLimitField); it != params.end()) {
      limitOut = std::stoull(std::string((*it).value));
    }
    if (const auto it = params.find(kOffsetField); it != params.end()) {
      offsetOut = std::stoull(std::string((*it).value));
    }
  }
}

http::response<http::string_body> BuildAndReturnOkResponse(
  const http::request<http::string_body>& req,
  const nlohmann::json& responseJson) 
{
  http::response<http::string_body> res{http::status::ok, req.version()};

  res.set(http::field::content_type, kJsonStrMime);

  res.body() = responseJson.dump();

  res.prepare_payload();

  return res;
}

http::response<http::string_body> BuildAndReturnNoContentResponse(
  const http::request<http::string_body>& req)
{
  http::response<http::string_body> res{http::status::no_content, req.version()};

  res.prepare_payload();

  return res;
}

std::string GetAuthHeader(const http::request<http::string_body>& req) {
  return std::string(req[http::field::authorization]);
}

bool IsValidToken(std::string_view authHeader) {
  return authHeader.starts_with(kAuthSchemeIdf);
}

std::string GetToken(std::string_view authHeader) {
  return std::string(authHeader.substr(kAuthSchemeIdfPrefixLen));
}

std::string ExtractPathParam(const Router::PathParams& params, std::string_view key) {
  for (const auto& [param_key, param_val] : params) {
    if (param_key == key) {
      return { param_val };
    }
  }
  throw std::invalid_argument("Missing required path parameter: " + std::string(key));
}


} // namespace nets::utils