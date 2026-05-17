#ifndef CLIENT_USER_SERVICE_H
#define CLIENT_USER_SERVICE_H

#include "Networking/i_rest_client.h"
#include "Repositories/i_local_user_repository.h"
#include "Services/i_client_user_service.h"

#include <string_view>

class ClientUserService : public IClientUserService {
public:
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
