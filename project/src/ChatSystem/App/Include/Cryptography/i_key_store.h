#ifndef I_KEY_STORE_H
#define I_KEY_STORE_H

#include <optional>

struct EncryptedKeyMaterial;

/**
 * @brief Repository interface for persisting encrypted private key material.
 * * Typically abstracts a local caching database, secure enclave, or file system
 * to store the user's encrypted identity.
 */
class IKeyStore {
public:
  virtual ~IKeyStore() = default;

  /**
   * @brief Saves the encrypted key material to local persistent storage.
   * @param encryptMaterial The material to be saved.
   */
  virtual void Store(const EncryptedKeyMaterial& encryptMaterial) = 0;

  /**
   * @brief Retrieves the encrypted key material from local storage.
   * @return An optional containing the material if it exists, std::nullopt otherwise.
   */
  virtual std::optional<EncryptedKeyMaterial> Load() = 0;

  /**
   * @brief Deletes the encrypted key material from local storage.
   */
  virtual void Clear() = 0;
};

#endif // I_KEY_STORE_H