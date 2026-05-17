#include "client_auth_service.h"

#include "API/auth_contract.h"
#include "db_model_json_mapper.h"


CachedUser ClientAuthService::Register(std::string_view login, std::string_view password) {
  const KeyPair key_pair = key_manager_obs_->GenerateAndProtectKeyPair(password);

  const nlohmann::json body = {
    {api::auth::fields::kLogin, login},
    {api::auth::fields::kPassword, password},
    {api::auth::fields::kPublicKey, key_pair.GetPublicKey()}
  };

  const HttpResponse response = rest_client_obs_->Post(api::auth::routes::kRegisterRoute, body);

  identity_repo_->Store(DBModelJsonMapper::ToIdentity(response.body));

  return DBModelJsonMapper::ToUser(response.body);
}

CachedUser ClientAuthService::Login(std::string_view login, std::string_view password) {
  const nlohmann::json body = {
  {api::auth::fields::kLogin, login},
  {api::auth::fields::kPassword, password},
  };

  const HttpResponse response = rest_client_obs_->Post(api::auth::routes::kLoginRoute, body);

  key_manager_obs_->GetUnlockedPrivateKey(password);

  const CachedIdentity logged_identity = DBModelJsonMapper::ToIdentity(response.body);

  session_token_ = logged_identity.session_token;

  identity_repo_->Store(logged_identity);

  return DBModelJsonMapper::ToUser(response.body);
}

void ClientAuthService::Logout() {

  const nlohmann::json body = {
  {api::auth::fields::kToken, session_token_},
  };

  rest_client_obs_->Post(api::auth::routes::kLoginRoute, body);

  session_token_.clear();

  identity_repo_->Clear();

  key_manager_obs_->DeleteProtectedKeys();
}




