#include "Networking/Controllers/user_controller.h"

#include "Networking/Controllers/utils.h"
#include "Networking/api_errors.h"
#include "Tags/user_tag.h"
#include "nlohmann/json.hpp"

#include <boost/beast.hpp>
#include <boost/url.hpp>

namespace {
  constexpr const char* kProfileIdField = "id";
  constexpr const char* kTagField = "tag";
  constexpr const char* kLoginField = "login";
  constexpr const char* kRoleField = "role";
  constexpr const char* kPublicKeyField = "public_key";

  constexpr std::string_view kPathParamId = "id";
  constexpr std::string_view kPathParamTag = "tag";

  constexpr std::size_t kDefaultSearchResultsLimit = 50;

  
  constexpr std::string_view kHandleSearchRoute = "/users/search";
  constexpr std::string_view kHandleHandleGetByIdRoute = "/users/{id}";
  constexpr std::string_view kHandleGetByTagRoute = "/users/tag/{tag}";
  constexpr std::string_view kHandleGetPublicKeyRoute = "/users/{id}/public-key";


} // namespace

namespace http = boost::beast::http;

UserController::UserController(IAuthService* authServiceObs, IUserService* userServiceObs)
  : auth_service_obs_(authServiceObs), user_service_obs_(userServiceObs) {
  if (!auth_service_obs_ || !user_service_obs_) {
    throw std::invalid_argument("UserController requires valid auth and user service observers.");
  }
}

// GET /users/search?q=&limit=&offset=
http::response<http::string_body> UserController::HandleSearchUsers(
  const http::request<http::string_body>& req,
  const Router::PathParams& params) const 
{
  try {
    const auto caller_id = GetAuthenticatedUserId(req);

    if (!caller_id) {
      return api::errors::Unauthorized(req);
    }

    auto query = ExtractSearchQuery(req);

    if (query.empty()) {
      return api::errors::BadRequest(req, "Missing search query parameter 'q'.");
    }

    std::size_t limit = kDefaultSearchResultsLimit;
    std::size_t offset = 0;

    netw::utils::ExtractPaginationLimitAndOffset(req, limit, offset);

    const auto found_users = user_service_obs_->Search(query, *caller_id, limit, offset);

    return netw::utils::BuildAndReturnOkResponse(req, FormatJsonOutput(found_users));
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

// GET /users/{id}
http::response<http::string_body> UserController::HandleGetById(
  const http::request<http::string_body>& req,
  const Router::PathParams& params) const 
{
  try {
    const auto caller_id = GetAuthenticatedUserId(req);

    if (!caller_id) {
      return api::errors::Unauthorized(req);
    }

    const UserId target_id = UserId::Reconstitute(netw::utils::ExtractPathParam(params, kPathParamId));

    const auto profile = user_service_obs_->GetById(target_id);

    if (!profile) {
      return api::errors::NotFound(req, "User not found.");
    }

    return netw::utils::BuildAndReturnOkResponse(req, FormatJsonOutput(*profile));
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

// GET /users/tag/{tag}
http::response<http::string_body> UserController::HandleGetByTag(
  const http::request<http::string_body>& req,
  const Router::PathParams& params) const
{
  try {
    const auto caller_id = GetAuthenticatedUserId(req);

    if (!caller_id) {
      return api::errors::Unauthorized(req);
    }

    const std::string target_tag = netw::utils::ExtractPathParam(params, kPathParamTag);

    const auto profile = user_service_obs_->GetByTag(tags::UserTag::Reconstitute(target_tag));

    if (!profile) {
      return api::errors::NotFound(req, "User not found.");
    }

    return netw::utils::BuildAndReturnOkResponse(req, FormatJsonOutput(*profile));
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

// GET /users/{id}/public-key
http::response<http::string_body> UserController::HandleGetPublicKey(
  const http::request<http::string_body>& req,
  const Router::PathParams& params) const 
{
  try {
    const auto caller_id = GetAuthenticatedUserId(req);

    if (!caller_id) {
      return api::errors::Unauthorized(req);
    }

    const UserId target_id = UserId::Reconstitute(netw::utils::ExtractPathParam(params, kPathParamId));

    auto public_key = user_service_obs_->GetPublicKey(target_id);

    if (!public_key) {
      return api::errors::NotFound(req, "Public key not found.");
    }

    nlohmann::json res_json = {
        {kPublicKeyField, *public_key}
    };

    return netw::utils::BuildAndReturnOkResponse(req, res_json);
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

std::optional<UserId> UserController::GetAuthenticatedUserId(const http::request<http::string_body>& req) const {
  const auto token = netw::utils::ExtractToken(req);

  if (!token) {
    return std::nullopt;
  }

  const auto user = auth_service_obs_->ValidateToken(*token);

  if (!user) {
    return std::nullopt;
  }

  return user->GetId();
}

nlohmann::json UserController::FormatJsonOutput(const std::vector<UserProfile>& userProfiles) {
  auto json_array = nlohmann::json::array();

  for (const auto& profile : userProfiles) {
    json_array.push_back(FormatJsonOutput(profile));
  }

  return json_array;
}

nlohmann::json UserController::FormatJsonOutput(const UserProfile& userProfile) {
  return nlohmann::json{
      {kProfileIdField, userProfile.id},
      {kTagField, userProfile.tag},
      {kLoginField, userProfile.login},
      {kRoleField, userProfile.role}
  };
}

std::string UserController::ExtractSearchQuery(const http::request<http::string_body>& req) {
  std::string query;

  if (auto rv = boost::urls::parse_origin_form(req.target())) {
    const auto url_params = rv->params();

    if (const auto it = url_params.find("q"); it != url_params.end()) {
      query = std::string((*it).value);
    }
  }

  return query;
}


void UserController::RegisterRoutes(Router& router) {
  router.AddRoute(http::verb::get, std::string(kHandleSearchRoute),
    [this](const auto& req, const auto& params) { return HandleSearchUsers(req, params); });

  router.AddRoute(http::verb::get, std::string(kHandleHandleGetByIdRoute),
    [this](const auto& req, const auto& params) { return HandleGetById(req, params); });

  router.AddRoute(http::verb::get, std::string(kHandleGetByTagRoute),
    [this](const auto& req, const auto& params) { return HandleGetByTag(req, params); });

  router.AddRoute(http::verb::get, std::string(kHandleGetPublicKeyRoute),
    [this](const auto& req, const auto& params) { return HandleGetPublicKey(req, params); });
}