#ifndef I_CLIENT_AUTH_SERVICE_H
#define I_CLIENT_AUTH_SERVICE_H

#include "Database/local_db_models.h"
#include "User/user.h"

#include <string_view>

class IClientAuthService {
public:
  virtual ~IClientAuthService() = default;

  virtual CachedUser Register(std::string_view login, std::string_view password) = 0;

  virtual CachedUser Login(std::string_view login, std::string_view password) = 0;

  virtual void Logout() = 0;
};

#endif // I_CLIENT_AUTH_SERVICE_H