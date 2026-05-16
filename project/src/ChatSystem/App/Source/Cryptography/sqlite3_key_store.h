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

  static bool TryExecuteStore(sqlite3_stmt* rawStmt);

  static bool TryExecuteLoad(sqlite3_stmt* rawStmt);

  bool TryPrepare(const char* sql, sqlite3_stmt** rawStmt) const;

  bool TryExecute(const char* sql, char*& errMsgOut) const;

  static void TryReadText(sqlite3_stmt* rawStmt, std::string& textOut, int col);

  static void TryReadBlob(sqlite3_stmt* rawStmt, std::vector<unsigned char>& vecBlobOut, int col);

};

#endif // SQLITE3_KEY_STORE_H