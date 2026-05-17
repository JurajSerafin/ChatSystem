#ifndef I_CLIENT_USER_SERVICE_H
#define I_CLIENT_USER_SERVICE_H
#include "User/user.h"

#include <vector>
#include <string_view>

class IClientUserService {
public:
  virtual ~IClientUserService() = default;

  virtual std::vector<User> Search(std::string_view query, std::size_t limit, std::size_t offset) = 0;

  virtual User GetById(const UserId& userId) = 0;

  virtual std::string GetPublicKey(const UserId& userId) = 0;
};

#endif // I_CLIENT_USER_SERVICE_H