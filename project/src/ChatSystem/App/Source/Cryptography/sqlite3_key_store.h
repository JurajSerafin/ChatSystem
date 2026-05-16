#ifndef SQLITE3_KEY_STORE_H
#define SQLITE3_KEY_STORE_H

#include "Cryptography/i_key_store.h"

#include <sqlite3.h>
#include <string_view>
#include <stdexcept>

#include <vector>

class Sqlite3KeyStore : public IKeyStore {
public:
  void Store(const EncryptedKeyMaterial& encryptMaterial) override;

  std::optional<EncryptedKeyMaterial> Load() override;

  void Clear() override;

  explicit Sqlite3KeyStore(std::string_view dbPath);

  ~Sqlite3KeyStore() override;

  Sqlite3KeyStore(const Sqlite3KeyStore&) = delete;
  Sqlite3KeyStore& operator=(const Sqlite3KeyStore&) = delete;

  Sqlite3KeyStore(Sqlite3KeyStore&& other) noexcept;
  Sqlite3KeyStore& operator=(Sqlite3KeyStore&& other) noexcept;

private:

  sqlite3* db_obs_;

  void InitSchema() const;

};

#endif // SQLITE3_KEY_STORE_H