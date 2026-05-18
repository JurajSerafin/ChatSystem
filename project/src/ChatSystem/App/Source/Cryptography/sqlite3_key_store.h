#ifndef SQLITE3_KEY_STORE_H
#define SQLITE3_KEY_STORE_H

#include "Cryptography/i_key_store.h"

#include <stdexcept>

#include <sqlite3.h>

/**
 * @brief Concrete implementation of the Key Store using SQLite3.
 * * Persists the encrypted private key material (salt, ciphertext, and KDF algorithm id)
 * safely to a local disk database file.
 */
class Sqlite3KeyStore : public IKeyStore {
public:
  /**
   * @brief Constructs the key store and initializes the SQLite database connection.
   * @param dbPath The file system path to the SQLite database file.
   */
  explicit Sqlite3KeyStore(const std::string& dbPath);

  /** @brief Closes the SQLite connection and cleans up resources. */
  ~Sqlite3KeyStore() override;

  // Enforce unique ownership of the SQLite connection pointer
  Sqlite3KeyStore(const Sqlite3KeyStore&) = delete;
  Sqlite3KeyStore& operator=(const Sqlite3KeyStore&) = delete;

  Sqlite3KeyStore(Sqlite3KeyStore&& other) noexcept;
  Sqlite3KeyStore& operator=(Sqlite3KeyStore&& other) noexcept;

  void Store(const EncryptedKeyMaterial& encryptMaterial) override;

  std::optional<EncryptedKeyMaterial> Load() override;

  void Clear() override;

private:
  /** @brief Raw pointer observing the active SQLite3 database connection. */
  sqlite3* db_obs_;

  /** @brief Ensures the local table structure (schema) for the key store exists on disk. */
  void InitSchema() const;
};

#endif // SQLITE3_KEY_STORE_H