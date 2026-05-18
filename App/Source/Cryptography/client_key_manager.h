#ifndef CLIENT_KEY_MANAGER_H
#define CLIENT_KEY_MANAGER_H

#include "Cryptography/i_client_encryption_service.h"
#include "Cryptography/i_client_key_manager.h"
#include "Cryptography/i_key_derivation_service.h"
#include "Cryptography/i_key_store.h"

#include <memory>

/**
 * @brief Concrete implementation of the client key manager.
 * * Orchestrates the underlying cryptographic, key derivation, and storage services
 * to handle the lifecycle of the user's encrypted identity keys.
 */
class ClientKeyManager : public IClientKeyManager {
public:
  /**
   * @brief Constructs a ClientKeyManager with required dependency injections.
   * @param cryptoServiceObs Observer pointer to the encryption engine.
   * @param keyDerivationService Unique pointer to the KDF service (takes ownership).
   * @param keyStore Unique pointer to the secure local storage repository (takes ownership).
   */
  ClientKeyManager(
    IClientEncryptionService* cryptoServiceObs,
    std::unique_ptr<IKeyDerivationService> keyDerivationService,
    std::unique_ptr<IKeyStore> keyStore
  );

  /**
   * @brief Generates a new key pair, derives a key from the password, and securely stores the encrypted private key.
   * @param password The user's plaintext password.
   * @return The raw, newly generated KeyPair.
   */
  KeyPair GenerateAndProtectKeyPair(std::string_view password) override;

  /**
   * @brief Derives a decryption key from the password to unlock the locally stored private key holds it in memory.
   * @param password The user's plaintext password.
   */
  void UnlockPrivateKey(std::string_view password) override;

  /**
   * @brief Retrieves the loaded private key.
   */
  std::string GetPrivateKey() override;

  /**
   * @brief Completely removes the protected key material from the injected key store.
   */
  void DeleteProtectedKeys() override;

private:
  IClientEncryptionService* crypto_service_obs_;

  std::unique_ptr<IKeyDerivationService> key_derivation_service_;
  std::unique_ptr<IKeyStore> key_store_;

  std::string loaded_private_key_;
};

#endif // !CLIENT_KEY_MANAGER_H