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

  for (auto&& user_json : res.body) {

    CachedUser cached_user = DBModelJsonMapper::ToUser(user_json);

    user_repo_obs_->Upsert(cached_user);

    search_results.emplace_back(std::move(cached_user));
  }

  return search_results;
}

CachedUser ClientUserService::GetById(const UserId& userId) {
  return client::services::utils::GetOrFetchById<CachedUser>(
    userId,
    user_repo_obs_,
    rest_client_obs_,
    [](auto&& body) {return DBModelJsonMapper::ToUser(body); },
    api::user::routes::kGetById,
    api::user::fields::kId
  );
}

std::string ClientUserService::GetPublicKey(const UserId& userId) {
  return GetById(userId).public_key;
}

