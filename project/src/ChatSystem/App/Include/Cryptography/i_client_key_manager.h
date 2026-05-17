#ifndef I_CLIENT_KEY_MANAGER_H
#define I_CLIENT_KEY_MANAGER_H

#include "key_pair.h"
#include <string_view>

class IClientKeyManager {
public:
  virtual void ~IClientKeyManager() = default;

  virtual KeyPair GenerateAndProtectKeyPair(std::string_view password) = 0;

  virtual std::string GetUnlockedPrivateKey(std::string_view password) = 0;

  virtual void DeleteProtectedKeys() = 0;
};

#endif // I_CLIENT_KEY_MANAGER_H