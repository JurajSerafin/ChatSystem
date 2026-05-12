#include "Networking/router.h"


inline void Router::AddRoute(http::verb method, const std::string& pathTemplate, HandlerFunc handler) {
  Route route;
  route.method = method;
  route.handler = std::move(handler);

  const auto request_template_view = urls::parse_origin_form(pathTemplate);

  if (!request_template_view) {
    throw std::invalid_argument("Invalid route template");
  }

  ProcessRouteTemplate(request_template_view, std::move(route));
}

inline http::response<http::string_body> Router::RouteRequest(const http::request<http::string_body>& req) {
  const auto request_template_view = urls::parse_origin_form(req.target());

  if (!request_template_view) {
    return HandleBadRequest(req);
  }

  return HandleRouting(req, request_template_view);
}

inline bool Router::IsDynamicParameter(std::string_view segment) {
  return segment.starts_with("{") && segment.ends_with("}");
}

inline void Router::ProcessDynamicParameter(Route& route, std::string_view param) {
  route.segments.emplace_back("");
  route.param_names.emplace_back(param.substr(1, param.length() - 2));
  route.is_dynamic.push_back(true);
}

inline void Router::ProcessStringMatch(Route& route, std::string&& param) {
  route.segments.emplace_back(std::move(param));
  route.param_names.emplace_back("");
  route.is_dynamic.push_back(false);
}

inline void Router::ProcessRouteTemplate(
  const boost::system::result<urls::url_view>& reqTemplateView,
  Route&& route) {
  for (auto&& seg : reqTemplateView.value().segments()) {
    if (IsDynamicParameter(seg)) {
      ProcessDynamicParameter(route, seg);
    }
    else {
      ProcessStringMatch(route, std::move(seg));
    }
  }
  routes_.push_back(std::move(route));
}

inline bool Router::TryExtractParams(
  PathParams& paramsOut,
  const std::vector<uint8_t>& isDynamicIndicators,
  const std::vector<std::string>& paramNames,
  const urls::segments_view& requestSegments,
  const std::vector<std::string>& segments
) {
  auto req_it = requestSegments.begin();

  for (std::size_t i = 0; i < segments.size(); ++i, ++req_it) {
    if (isDynamicIndicators[i] != 0) {
      paramsOut.emplace_back(paramNames[i], *req_it);
    }
    else if (segments[i] != *req_it) {
      return false;
    }
  }

  return true;
}

inline http::response<http::string_body> Router::HandleRouting(
  const http::request<http::string_body>& req,
  const boost::system::result<urls::url_view>& reqTemplateView
) {
  const auto request_segments = reqTemplateView.value().segments();

  PathParams extracted_params;
  extracted_params.reserve(kAverageRouteParamCount);

  for (auto&& [method, segments, param_names, is_dynamic, handler] : routes_) {
    if (req.method() != method) {
      continue;
    }

    if (request_segments.size() != segments.size()) {
      continue;
    }

    extracted_params.clear();

    if (TryExtractParams(extracted_params, is_dynamic, param_names, request_segments, segments)) {
      return handler(req, extracted_params);
    }
  }

  return HandleNotFound(req);
}

inline http::response<http::string_body> Router::HandleNotFound(const http::request<http::string_body>& req) {
  http::response<http::string_body> res{ http::status::not_found, req.version() };
  res.body() = R"({"error": "Endpoint not found."})";
  res.prepare_payload();
  return res;
}

inline http::response<http::string_body> Router::HandleBadRequest(const http::request<http::string_body>& req) {
  http::response<http::string_body> res{ http::status::bad_request, req.version() };
  res.body() = R"({"error": "Malformed URL."})";
  res.prepare_payload();
  return res;
}