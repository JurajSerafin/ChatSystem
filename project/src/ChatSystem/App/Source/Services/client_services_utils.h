#ifndef CLIENT_SERVICES_UTILS_H
#define CLIENT_SERVICES_UTILS_H

#include "Networking/http_response.h"
#include "Networking/i_rest_client.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <concepts>

/**
 * @brief Utility namespace containing shared networking routines and generic Cache-Aside template algorithms.
 */
namespace client::services::utils {

  /** @brief Constructs a formatted query string for paginated search endpoints. */
  std::string ConstructSearchQuery(
    std::string_view query, std::size_t limit, std::size_t offset,
    std::string_view route, std::string_view query_params
  );

  /** @brief Safely replaces a dynamic parameter token (e.g., "{id}") within a string route template. */
  std::string ResolveIdRoute(std::string_view routeTemplate, const std::string& idValue, std::string_view idParamName);

  /** @brief Constructs a standard paginated URL route without search parameters. */
  std::string ConstructFetchArrayQuery(std::size_t limit, std::size_t offset, std::string_view route);

  /**
   * @brief A generic implementation of the Cache-Aside pattern for single entities.
   * * Attempts to load an entity from the local repository; if it misses, falls back to the network,
   * maps the JSON response, and persists it to the cache before returning.
   * * @tparam TElementModel The concrete database model type (e.g., CachedUser).
   * @tparam TId The domain ID wrapper type.
   * @tparam TRepo The repository class capable of loading from and upserting into a repository.
   * @tparam TMapperFunc A callable that translates nlohmann::json into TElementModel.
   */
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
    const TId& id, TRepo* repoObs, IRestClient* restClientObs,
    TMapperFunc mapperFunc, std::string_view route, std::string_view idParamName
  )
  {
    const std::string id_str = id.ToString();
    std::optional<TElementModel> cached_model_opt = repoObs->FindById(id_str);

    if (cached_model_opt.has_value()) {
      return *cached_model_opt;
    }

    const std::string dynamic_id_route = ResolveIdRoute(route, id_str, idParamName);
    const HttpResponse res = restClientObs->Get(dynamic_id_route);
    cached_model_opt = mapperFunc(res.body);
    repoObs->Upsert(*cached_model_opt);

    return *cached_model_opt;
  }

  /**
   * @brief A generic implementation of the Cache-Aside pattern for collections/arrays.
   * * Leverages custom lambdas to evaluate complex cached arrays and batch-store network results.
   * * @tparam TElementModel The concrete database model type to populate the vector.
   * @tparam TId The domain ID wrapper type determining the collection's scope.
   * @tparam TRepo The repository class.
   * @tparam TMapperFunc A callable that translates nlohmann::json into std::vector<TElementModel>.
   */
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
    const TId& id, TRepo* repoObs, IRestClient* restClientObs,
    TMapperFunc mapperFunc, std::string_view route, std::string_view idParamName,
    std::function<std::vector<TElementModel>(std::string_view)> cacheLookupFunc,
    std::function<void(const std::vector<TElementModel>&)> cacheStorageFunc
  )
  {
    const std::string id_str = id.ToString();
    std::vector<TElementModel> cached_list = cacheLookupFunc(id_str);

    if (!cached_list.empty()) {
      return cached_list;
    }

    std::string dynamic_route = ResolveIdRoute(route, id_str, idParamName);
    const HttpResponse res = restClientObs->Get(dynamic_route);
    std::vector<TElementModel> fetched_list = mapperFunc(res.body);
    cacheStorageFunc(fetched_list);

    return fetched_list;
  }
} // namespace client::services::utils

#endif // CLIENT_SERVICES_UTILS_H