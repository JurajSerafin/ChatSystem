#ifndef CLIENT_AUTH_SERVICE_H
#define CLIENT_AUTH_SERVICE_H

#include "Networking/i_rest_client.h"
#include "Repositories/i_local_identity_repository.h"
#include "Repositories/i_local_user_repository.h"
#include "Services/i_client_auth_service.h"
#include "Cryptography/i_client_key_manager.h"


class ClientAuthService : public IClientAuthService {
public:
  explicit ClientAuthService(
    IRestClient* restClientObs,
    IClientKeyManager* keyManagerObs,
    ILocalIdentityRepository* identityRepoObs
  );

  CachedUser Register(std::string_view login, std::string_view password) override;

  CachedUser Login(std::string_view login, std::string_view password) override;

  void Logout() override;

private:
  IRestClient* rest_client_obs_;
  IClientKeyManager* key_manager_obs_;
  ILocalIdentityRepository* identity_repo_;

  std::string session_token_;
};

inline ClientAuthService::ClientAuthService(
  IRestClient* restClientObs,
  IClientKeyManager* keyManagerObs,
  ILocalIdentityRepository* identityRepoObs
) : rest_client_obs_(restClientObs),
  key_manager_obs_(keyManagerObs),
  identity_repo_(identityRepoObs) {}

#endif // CLIENT_AUTH_SERVICE_H

