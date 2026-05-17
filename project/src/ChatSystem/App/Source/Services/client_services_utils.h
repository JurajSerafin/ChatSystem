#ifndef CLIENT_SERVICES_UTILS_H
#define CLIENT_SERVICES_UTILS_H

#include "Networking/http_response.h"
#include "Networking/i_rest_client.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <concepts>

namespace client::services::utils{
  std::string ConstructSearchQuery(
    std::string_view query,
    std::size_t limit,
    std::size_t offset,
    std::string_view route,
    std::string_view query_params
  );

  std::string ResolveIdRoute(std::string_view routeTemplate, const std::string& idValue, std::string_view idParamName);

  std::string ConstructFetchArrayQuery(std::size_t limit, std::size_t offset, std::string_view route);

  template<
    typename TElementModel,
    typename TId,
    typename TRepo,
    typename TMapperFunc
  >
    requires requires(TMapperFunc mapper, const nlohmann::json& json) {
      { mapper(json) } -> std::same_as<TElementModel>;
  }
  TElementModel GetOrFetchById(
    const TId& id,
    TRepo* repoObs,
    IRestClient* restClientObs,
    TMapperFunc mapperFunc,
    std::string_view route,
    std::string_view idParamName
  )
  {
    const std::string id_str = id.ToString();
    std::optional<TElementModel> cached_model_opt = repoObs->FindById(id_str);

    // Cache Hit
    if (cached_model_opt.has_value()) {
      return *cached_model_opt;
    }

    const std::string dynamic_id_route = ResolveIdRoute(route, id_str, idParamName);

    const HttpResponse res = restClientObs->Get(dynamic_id_route);

    cached_model_opt = mapperFunc(res.body);

    repoObs->Upsert(*cached_model_opt);

    return *cached_model_opt;
  }

  template<
    typename TElementModel,
    typename TId,
    typename TRepo,
    typename TMapperFunc
  >
    requires requires(TMapperFunc mapper, const nlohmann::json& json) {
      { mapper(json) } -> std::same_as<std::vector<TElementModel>>;
  }
  std::vector<TElementModel> GetOrFetchCollection(
    const TId& id,
    TRepo* repoObs,
    IRestClient* restClientObs,
    TMapperFunc mapperFunc,
    std::string_view route,
    std::string_view idParamName,
    std::function<std::vector<TElementModel>(std::string_view)> cacheLookupFunc,
    std::function<void(const std::vector<TElementModel>&)> cacheStorageFunc
  )
  {
    const std::string id_str = id.ToString();

    std::vector<TElementModel> cached_list = cacheLookupFunc(id_str);

    // Cache Hit
    if (!cached_list.empty()) {
      return cached_list;
    }

    // Cache Miss
    std::string dynamic_route = ResolveIdRoute(route, id_str, idParamName);

    const HttpResponse res = restClientObs->Get(dynamic_route);

    std::vector<TElementModel> fetched_list = mapperFunc(res.body);

    cacheStorageFunc(fetched_list);

    return fetched_list;
  }

} // namespace client::services::utils


#endif // CLIENT_SERVICES_UTILS_H
