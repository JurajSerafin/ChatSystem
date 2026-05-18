#ifndef CLIENT_USER_SERVICE_H
#define CLIENT_USER_SERVICE_H

#include "Networking/i_rest_client.h"
#include "Repositories/i_local_user_repository.h"
#include "Services/i_client_user_service.h"

#include <string_view>

/**
 * @brief Concrete implementation of the client user service.
 * * Handles fetching and persisting global user directories and specific profiles,
 * ensuring the application has instant access to cached public keys.
 */
class ClientUserService : public IClientUserService {
public:
  /**
   * @brief Constructs the user service with required dependencies.
   * @param restClientObs Pointer to the networking client.
   * @param userRepoObs Pointer to the local user cache repository.
   */
  explicit ClientUserService(IRestClient* restClientObs, ILocalUserRepository* userRepoObs);

  std::vector<CachedUser> Search(std::string_view query, std::size_t limit, std::size_t offset) override;
  CachedUser GetById(const UserId& userId) override;
  std::string GetPublicKey(const UserId& userId) override;

private:
  IRestClient* rest_client_obs_;
  ILocalUserRepository* user_repo_obs_;
};

inline ClientUserService::ClientUserService(IRestClient* restClientObs, ILocalUserRepository* userRepoObs) : rest_client_obs_(restClientObs), user_repo_obs_(userRepoObs) {}

#endif // CLIENT_USER_SERVICE_H