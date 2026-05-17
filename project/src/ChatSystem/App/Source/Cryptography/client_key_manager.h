#ifndef CLIENT_KEY_MANAGER_H
#define CLIENT_KEY_MANAGER_H

#include "Cryptography/i_client_encryption_service.h"
#include "Cryptography/i_client_key_manager.h"
#include "Cryptography/i_key_derivation_service.h"
#include "Cryptography/i_key_store.h"

#include <memory>

class ClientKeyManager : public IClientKeyManager {
public:
  ClientKeyManager(
    IClientEncryptionService* cryptoServiceObs,
    std::unique_ptr<IKeyDerivationService> keyDerivationService,
    std::unique_ptr<IKeyStore> keyStore
  );

  KeyPair GenerateAndProtectKeyPair(std::string_view password) override;

  std::string GetUnlockedPrivateKey(std::string_view password) override;

  void DeleteProtectedKeys() override;

private:
  IClientEncryptionService* crypto_service_obs_;

  std::unique_ptr<IKeyDerivationService> key_derivation_service_;
  std::unique_ptr<IKeyStore> key_store_;
};

#endif // !CLIENT_KEY_MANAGER_H
