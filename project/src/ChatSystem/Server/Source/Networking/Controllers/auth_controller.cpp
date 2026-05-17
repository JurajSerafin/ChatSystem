#include "Networking/Controllers/auth_controller.h"
#include "Networking/api_errors.h"
#include "Networking/Controllers/utils.h"
#include "Api/auth_contract.h"
#include "Api/user_contract.h" // Needed to write user fields into the response

#include <stdexcept>
#include <format>

// POST /auth/register
http::response<http::string_body> AuthController::HandleRegister(
  const http::request<http::string_body>& req,
  const Router::PathParams&) const
{
  try {
    auto body = nlohmann::json::parse(req.body());
    const std::string login = ExtractRequiredString(body, api::auth::fields::kLogin);
    const std::string password = ExtractRequiredString(body, api::auth::fields::kPassword);
    const std::string public_key = ExtractRequiredString(body, api::auth::fields::kPublicKey);

    const auto auth_result = auth_service_obs_->RegisterUser(login, password, public_key);

    const auto session_user = *user_service_obs_->GetById(auth_result.GetUserId());

    nlohmann::json response_json = FormatAuthResponse(auth_result.GetToken(), session_user);

    return netw::utils::BuildAndReturnOkResponse(req, response_json);
  }
  catch (const nlohmann::json::exception&) {
    return api::errors::BadRequest(req, "Invalid JSON format.");
  }
  catch (const std::invalid_argument& e) {
    return api::errors::BadRequest(req, e.what());
  }
  catch (const std::exception& e) {
    return api::errors::Conflict(req, e.what());
  }
}

// POST /auth/login
http::response<http::string_body> AuthController::HandleLogin(
  const http::request<http::string_body>& req,
  const Router::PathParams&) const
{
  try {
    auto body = nlohmann::json::parse(req.body());
    const std::string login = ExtractRequiredString(body, api::auth::fields::kLogin);
    const std::string password = ExtractRequiredString(body, api::auth::fields::kPassword);

    const auto auth_result = auth_service_obs_->Login(login, password);

    auto session_user = user_service_obs_->GetById(auth_result.GetUserId());

    nlohmann::json response_json = FormatAuthResponse(
      auth_result.GetToken(),
      *session_user
    );

    return netw::utils::BuildAndReturnOkResponse(req, response_json);
  }
  catch (const nlohmann::json::exception& e) {
    return api::errors::BadRequest(req, "Invalid JSON format");
  }
  catch (const std::invalid_argument& e) {
    return api::errors::BadRequest(req, e.what());
  }
  catch (const std::runtime_error& e) {
    return api::errors::Unauthorized(req, "Invalid credentials.");
  }
}

// POST /auth/logout
http::response<http::string_body> AuthController::HandleLogout(
  const http::request<http::string_body>& req,
  const Router::PathParams&) const
{
  try {
    const auto token = netw::utils::ExtractToken(req);

    if (!token) {
      return api::errors::Unauthorized(req, "Missing or invalid token.");
    }

    auth_service_obs_->Logout(*token);

    return netw::utils::BuildAndReturnNoContentResponse(req);
  }
  catch (const std::exception& e) {
    return api::errors::InternalServerError(req);
  }
}

// PUT /auth/password
http::response<http::string_body> AuthController::HandleChangePassword(
  const http::request<http::string_body>& req,
  const Router::PathParams&) const
{
  try {
    const auto token = netw::utils::ExtractToken(req);

    if (!token) {
      return api::errors::Unauthorized(req);
    }

    const auto user_id = auth_service_obs_->ValidateToken(*token);

    if (!user_id) {
      return api::errors::Unauthorized(req, "Session expired.");
    }

    ExtractAndChangePassword(nlohmann::json::parse(req.body()), user_id->GetId());

    return netw::utils::BuildAndReturnNoContentResponse(req);
  }
  catch (const nlohmann::json::exception&) {
    return api::errors::BadRequest(req, "Invalid JSON format.");
  }
  catch (const std::invalid_argument& e) {
    return api::errors::BadRequest(req, e.what());
  }
  catch (const std::exception& e) {
    return api::errors::BadRequest(req, e.what());
  }
}

std::string AuthController::ExtractRequiredString(const nlohmann::json& body, std::string_view fieldName) {
  if (!body.contains(fieldName)) {
    throw std::invalid_argument(std::format("Missing required field: {}", fieldName));
  }

  auto value = body.at(fieldName).get<std::string>();

  if (value.empty()) {
    throw std::invalid_argument(std::format("Field cannot be empty: {}", fieldName));
  }

  return value;
}

nlohmann::json AuthController::FormatAuthResponse(const std::string& token, const UserProfile& userProfile) {
  return {
    {api::auth::fields::kToken, token},
    {api::user::fields::kId, userProfile.id},
    {api::user::fields::kLogin, userProfile.login},
    {api::user::fields::kTag, userProfile.tag},
  };
}

void AuthController::ExtractAndChangePassword(const nlohmann::json& passChangeBody, const UserId& userId) const {
  const auto old_pass = ExtractRequiredString(passChangeBody, api::auth::fields::kOldPassword);
  const auto new_pass = ExtractRequiredString(passChangeBody, api::auth::fields::kNewPassword);

  auth_service_obs_->ChangePassword(userId, old_pass, new_pass);
}

void AuthController::RegisterRoutes(Router& router) {
  router.AddRoute(http::verb::post, std::string{ api::auth::routes::kRegisterRoute },
    [this](const auto& req, const auto& params) {return HandleRegister(req, params); }
  );

  router.AddRoute(http::verb::post, std::string{ api::auth::routes::kLoginRoute },
    [this](const auto& req, const auto& params) {return HandleLogin(req, params); }
  );

  router.AddRoute(http::verb::post, std::string{ api::auth::routes::kLogoutRoute },
    [this](const auto& req, const auto& params) {return HandleLogout(req, params); }
  );

  router.AddRoute(http::verb::put, std::string{ api::auth::routes::kChangePasswordRoute },
    [this](const auto& req, const auto& params) {return HandleChangePassword(req, params); }
  );
};