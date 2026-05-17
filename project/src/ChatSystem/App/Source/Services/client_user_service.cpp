#include "client_user_service.h"

#include "API/user_contract.h"
#include "db_model_json_mapper.h"
#include "client_services_utils.h"

#include <format>

std::vector<CachedUser> ClientUserService::Search(std::string_view query, std::size_t limit, std::size_t offset) {
  const std::string path = client::services::utils::ConstructSearchQuery(
    query, limit, offset,
    api::user::routes::kSearch, api::user::query_params::kQuery
  );

  const HttpResponse res = rest_client_obs_->Get(path);

  std::vector<CachedUser> search_results;

  for (const auto& user_json : res.body) {

    CachedUser cached_user = DBModelJsonMapper::ToUser(user_json);

    user_repo_obs_->Upsert(cached_user);

    search_results.emplace_back(std::move(cached_user));
  }

  return search_results;
}

CachedUser ClientUserService::GetById(const UserId& userId) {
  std::optional<CachedUser> cached_user = user_repo_obs_->FindById(userId.ToString());

  // Cache Hit
  if (cached_user.has_value()) {
    return *cached_user;
  }

  // Cache Miss
  std::string dynamic_id_route = client::services::utils::ResolveIdRoute(
    api::user::routes::kGetById, userId.ToString(), api::user::fields::kId
  );


  const HttpResponse res = rest_client_obs_->Get(dynamic_id_route);

  cached_user = DBModelJsonMapper::ToUser(res.body);

  user_repo_obs_->Upsert(*cached_user);

  return *cached_user;
}

std::string ClientUserService::GetPublicKey(const UserId& userId) {
  std::optional<CachedUser> cached_user = user_repo_obs_->FindById(userId.ToString());

  // Cache Hit
  if (cached_user.has_value()) {
    return cached_user->public_key;
  }

  // Cache Miss
  std::string dynamic_id_route = client::services::utils::ResolveIdRoute(
    api::user::routes::kGetById, userId.ToString(), api::user::fields::kId
  );
  const HttpResponse res = rest_client_obs_->Get(dynamic_id_route);

  cached_user = DBModelJsonMapper::ToUser(res.body);

  user_repo_obs_->Upsert(*cached_user);

  return cached_user->public_key;
}

