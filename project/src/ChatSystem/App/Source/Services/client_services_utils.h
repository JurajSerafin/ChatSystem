#ifndef CLIENT_SERVICES_UTILS_H
#define CLIENT_SERVICES_UTILS_H

#include <string>
#include <string_view>

namespace client::services::utils{
  std::string ConstructSearchQuery(
    std::string_view query,
    std::size_t limit,
    std::size_t offset,
    std::string_view route,
    std::string_view query_params
  );

  std::string ResolveIdRoute(std::string_view routeTemplate, const std::string& idValue, std::string_view idParamName);

} // namespace client::services::utils


#endif // CLIENT_SERVICES_UTILS_H
