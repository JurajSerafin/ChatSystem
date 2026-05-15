#include "Networking/router.h"

#include "Networking/Controllers/utils.h"
#include "boost/url/parse.hpp"

#include <sstream>

void Router::AddRoute(http::verb method, const std::string& pathTemplate, HandlerFunc handler) {
  Route route;

  route.method = method;
  route.handler = std::move(handler);

  ProcessRouteTemplate(pathTemplate, route);

  routes_.push_back(std::move(route));
}


std::string Router::ExtractTarget(const http::request<http::string_body>& req) {
  return { req.target().data(), req.target().size() };
}

std::string Router::ExtractPath(const std::string& target) {
  return target.substr(0, target.find('?'));
}

void Router::DiscardLeadingEmptyString(const std::string& path, std::istream& pathStream, std::string& segment) {
  if (!path.empty() && path.front() == '/') {
    std::getline(pathStream, segment, '/');
  }
}

std::vector<std::string> Router::GetParsedRequestSegments(const std::string& path, std::istream& pathStream, std::string& segment) {
  std::vector<std::string> request_segments;

  while (std::getline(pathStream, segment, '/')) {
    request_segments.push_back(segment);
  }

  return request_segments;
}

http::response<http::string_body> Router::RouteRequest(const http::request<http::string_body>& req) {
  if (!boost::urls::parse_origin_form(req.target())) {
    return HandleBadRequest(req);
  }

  const std::string target_str = ExtractTarget(req);

  const std::string path = ExtractPath(target_str);

  std::stringstream path_stream(path);

  std::string segment;

  DiscardLeadingEmptyString(path, path_stream, segment);

  const std::vector<std::string> request_segments = GetParsedRequestSegments(path, path_stream, segment);

  for (auto&& [method, segments, param_names, is_dynamic, handler] : routes_) {
    if (req.method() != method
      || request_segments.size() != segments.size()) {
      continue;
    }

    if (PathParams params; TryExtractParams(params, is_dynamic, param_names, request_segments, segments)) {
      return handler(req, params);
    }
  }

  return HandleNotFound(req);
}

bool Router::IsDynamicParameter(std::string_view segment) {
  return segment.starts_with("{") && segment.ends_with("}");
}

void Router::ProcessDynamicParameter(Route& route, std::string_view param) {
  route.segments.emplace_back("");

  route.param_names.emplace_back(param.substr(1, param.length() - 2));

  route.is_dynamic.push_back(true);
}

void Router::ProcessStringMatch(Route& route, std::string&& param) {
  route.segments.emplace_back(std::move(param));

  route.param_names.emplace_back("");

  route.is_dynamic.push_back(false);
}

void Router::ProcessRouteTemplate(const std::string& pathTemplate, Route& route) {
  std::stringstream pathTemplateStream(pathTemplate);
  std::string segment;

  DiscardLeadingEmptyString(pathTemplate, pathTemplateStream, segment);

  while (std::getline(pathTemplateStream, segment, '/')) {
    if (IsDynamicParameter(segment)) {
      ProcessDynamicParameter(route, segment);
    }
    else {
      ProcessStringMatch(route, std::move(segment));
    }
  }
}

bool Router::TryExtractParams(
  PathParams& paramsOut,
  const std::vector<uint8_t>& isDynamicIndicators,
  const std::vector<std::string>& paramNames,
  const std::vector<std::string>& requestSegments,
  const std::vector<std::string>& segments
) {
  for (std::size_t i = 0; i < segments.size(); ++i) {
    if (isDynamicIndicators[i] != 0) {
      paramsOut.emplace_back(paramNames[i], requestSegments[i]);
    }
    else if (segments[i] != requestSegments[i]) {
      return false;
    }
  }
  return true;
}

http::response<http::string_body> Router::HandleNotFound(const http::request<http::string_body>& req) {
  http::response<http::string_body> res{ http::status::not_found, req.version() };

  res.body() = R"({"error": "Endpoint not found."})";

  res.prepare_payload();

  return res;
}

http::response<http::string_body> Router::HandleBadRequest(const http::request<http::string_body>& req) {
  http::response<http::string_body> res{ http::status::bad_request, req.version() };

  res.body() = R"({"error": "Malformed URL."})";

  res.prepare_payload();

  return res;
}