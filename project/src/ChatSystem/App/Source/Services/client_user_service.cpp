#include "client_user_service.h"

#include "API/user_contract.h"
#include "db_model_json_mapper.h"

#include <format>

std::vector<CachedUser> ClientUserService::Search(std::string_view query, std::size_t limit, std::size_t offset) {
  const std::string path = ConstructSearchQuery(query, limit, offset);

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
  std::string dynamic_id_route = ResolveIdRoute(api::user::routes::kGetById, userId.ToString());

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
  std::string dynamic_id_route = ResolveIdRoute(api::user::routes::kGetById, userId.ToString());

  const HttpResponse res = rest_client_obs_->Get(dynamic_id_route);

  cached_user = DBModelJsonMapper::ToUser(res.body);

  user_repo_obs_->Upsert(*cached_user);

  return cached_user->public_key;
}

std::string ClientUserService::ConstructSearchQuery(std::string_view query, std::size_t limit, std::size_t offset) {
  return std::format("{}?{}={}&limit={}&offset={}",
    api::user::routes::kSearch,
    api::user::query_params::kQuery,
    query,
    limit,
    offset
  );
}

std::string ClientUserService::ResolveIdRoute(std::string_view route_template, const std::string& id_value) {
  std::string resolved{route_template};
  const std::string target = "{" + std::string(api::user::path_params::kId) + "}";

  if (size_t pos = resolved.find(target); pos != std::string::npos) {
      resolved.replace(pos, target.length(), id_value);
  }
  return resolved;
}
