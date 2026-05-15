#include "Networking/Controllers/auth_controller.h"
#include "Networking/api_errors.h"
#include "Networking/Controllers/utils.h"
#include <stdexcept>
#include <format>

namespace {
  constexpr std::string_view kLoginField = "login";
  constexpr std::string_view kPasswordField = "password";
  constexpr std::string_view kPublicKeyField = "public_key";
  constexpr std::string_view kOldPasswordField = "old_password";
  constexpr std::string_view kNewPasswordField = "new_password";
  constexpr std::string_view kTokenField = "token";


  constexpr std::string_view kRegisterRoute = "/auth/register";
  constexpr std::string_view kLoginRoute = "/auth/login";
  constexpr std::string_view kLogoutRoute = "/auth/logout";
  constexpr std::string_view kChangePasswordRoute = "/auth/password";
}  // namespace


// POST /auth/register
http::response<http::string_body> AuthController::HandleRegister(
  const http::request<http::string_body>& req,
  const Router::PathParams&) const
{
  try {
    const std::string session_token = RegisterAndGetSessionToken(nlohmann::json::parse(req.body()));

    return netw::utils::BuildAndReturnOkResponse(req, FormatTokenResponse(session_token));
  }
  catch (const nlohmann::json::exception&) {
    return api::errors::BadRequest(req, "Invalid JSON format.");
  }
  catch (const std::invalid_argument& e) {
    //  empty/missing field errors
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
    const std::string session_token = LoginAndGetSessionToken(nlohmann::json::parse(req.body()));
    return netw::utils::BuildAndReturnOkResponse(req, FormatTokenResponse(session_token));
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

nlohmann::json AuthController::FormatTokenResponse(const std::string& token) {
  return {
    {kTokenField, token}
  };
}

std::string AuthController::RegisterAndGetSessionToken(const nlohmann::json& authDataBody) const {
  const std::string login = ExtractRequiredString(authDataBody, kLoginField);
  const std::string password = ExtractRequiredString(authDataBody, kPasswordField);
  const std::string public_key = ExtractRequiredString(authDataBody, kPublicKeyField);

  return auth_service_obs_->RegisterUser(login, password, public_key).GetToken();
}

std::string AuthController::LoginAndGetSessionToken(const nlohmann::json& authDataBody) const {
  const std::string login = ExtractRequiredString(authDataBody, kLoginField);
  const std::string password = ExtractRequiredString(authDataBody, kPasswordField);

  return auth_service_obs_->Login(login, password).GetToken();
}

void AuthController::ExtractAndChangePassword(const nlohmann::json& passChangeBody, const UserId& userId) const {

  const auto old_pass = ExtractRequiredString(passChangeBody, kOldPasswordField);

  const auto new_pass = ExtractRequiredString(passChangeBody, kNewPasswordField);

  auth_service_obs_->ChangePassword(userId, old_pass, new_pass);

}

void AuthController::RegisterRoutes(Router& router) {
  router.AddRoute(http::verb::post, std::string{kRegisterRoute},
    [this](const auto& req, const auto& params) {return HandleRegister(req, params); }
  );

  router.AddRoute(http::verb::post, std::string{ kLoginRoute },
    [this](const auto& req, const auto& params) {return HandleLogin(req, params); }
  );

  router.AddRoute(http::verb::post, std::string{ kLogoutRoute },
    [this](const auto& req, const auto& params) {return HandleLogout(req, params); }
  );

  router.AddRoute(http::verb::post, std::string{ kChangePasswordRoute },
    [this](const auto& req, const auto& params) {return HandleChangePassword(req, params); }
  );
};
