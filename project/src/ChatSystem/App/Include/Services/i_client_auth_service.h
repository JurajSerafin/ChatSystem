#ifndef I_CLIENT_AUTH_SERVICE_H
#define I_CLIENT_AUTH_SERVICE_H

#include "User/user.h"
#include <string_view>

class IClientAuthService {
public:
  virtual ~IClientAuthService() = default;

  virtual User Register(std::string_view login, std::string_view password) = 0;

  virtual User Login(std::string_view login, std::string_view password) = 0;

  virtual void Logout() = 0;
};

#endif // I_CLIENT_AUTH_SERVICE_H