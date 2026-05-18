#include "client_services_utils.h"
#include <format>


namespace client::services::utils {
  std::string ConstructSearchQuery(
    std::string_view query,
    std::size_t limit,
    std::size_t offset,
    std::string_view route,
    std::string_view query_params
  ) {
    return std::format("{}?{}={}&limit={}&offset={}",
      route,
      query_params,
      query,
      limit,
      offset
    );
  }

  std::string ResolveIdRoute(std::string_view routeTemplate, const std::string& idValue, std::string_view idParamName) {
    
    std::string resolved{ routeTemplate };
    const std::string target = "{" + std::string(idParamName) + "}";

    if (size_t pos = resolved.find(target); pos != std::string::npos) {
      resolved.replace(pos, target.length(), idValue);
    }
    return resolved;
  }

  std::string ConstructFetchArrayQuery(std::size_t limit, std::size_t offset, std::string_view route) {
    return std::format("{}&limit={}&offset={}",
      route,
      limit,
      offset
    );
  }

} // namespace client::services::utils