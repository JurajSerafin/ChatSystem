#ifndef I_CLIENT_KEY_MANAGER_H
#define I_CLIENT_KEY_MANAGER_H

#include "key_pair.h"
#include <string_view>

/**
 * @brief High-level orchestrator for generating, protecting, and unlocking identity keys.
 * 
 * Bridges the Key Derivation, Encryption, and Key Store services to manage
 * the lifecycle of the user's long-term identity keys.
 */
class IClientKeyManager {
public:
  virtual ~IClientKeyManager() = default;

  /**
   * @brief Generates a new key pair, encrypts the private key using the password, and stores it locally.
   * @param password The user's plaintext password.
   * @return The newly generated KeyPair.
   */
  virtual KeyPair GenerateAndProtectKeyPair(std::string_view password) = 0;

  /**
   * @brief Derives a key from the password to decrypt and return the user's local private key.
   * @param password The user's plaintext password.
   * @return The raw, unencrypted private key string.
   */
  virtual std::string GetUnlockedPrivateKey(std::string_view password) = 0;

  /**
   * @brief Wipes the locally stored encrypted key material from disk.
   */
  virtual void DeleteProtectedKeys() = 0;
};

#endif // I_CLIENT_KEY_MANAGER_H