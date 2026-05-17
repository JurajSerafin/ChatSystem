#include "sqlite3_key_store.h"
#include "Cryptography/encrypted_key_material.h"
#include "Infrastructure/Sqlite3/sqlite3_utils.h"

#include <memory>
#include <utility>
#include <chrono>
#include <optional>

#include <format>

Sqlite3KeyStore::Sqlite3KeyStore(Sqlite3KeyStore&& other) noexcept
  : db_obs_(std::exchange(other.db_obs_, nullptr)) {}

Sqlite3KeyStore& Sqlite3KeyStore::operator=(Sqlite3KeyStore&& other) noexcept {
  if (this != &other) {

    if (db_obs_) {
      sqlite3_close(db_obs_);
    }

    db_obs_ = std::exchange(other.db_obs_, nullptr);
  }

  return *this;
}

Sqlite3KeyStore::Sqlite3KeyStore(const std::string& dbPath) {
  if (sqlite3_open(dbPath.c_str(), &db_obs_) != SQLITE_OK) {
    std::string error = sqlite3_errmsg(db_obs_);

    sqlite3_close(db_obs_);

    throw std::runtime_error("Failed to open SQLite database: " + error);
  }
  InitSchema();
}

Sqlite3KeyStore::~Sqlite3KeyStore() {
  if (db_obs_) {
    sqlite3_close(db_obs_);
  }
}

void Sqlite3KeyStore::Store(const EncryptedKeyMaterial& encryptMaterial) {
  constexpr auto sql = R"(
    INSERT OR REPLACE INTO key_store (id, algorithm, encrypted_key, salt, stored_at) 
    VALUES (1, ?, ?, ?, ?);
  )";

  sqlite3_stmt* raw_stmt = nullptr;

  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Failed to prepare Store statement: " + std::string(sqlite3_errmsg(db_obs_)));
  }

  const Sqlite3Utils::ScopedStmt stmt { raw_stmt };

  const auto now = std::chrono::system_clock::now();

  Sqlite3Utils::BindStr(stmt.get(), encryptMaterial.algorithm, 1);
  Sqlite3Utils::BindByteVector(stmt.get(), encryptMaterial.encrypted_key, 2);
  Sqlite3Utils::BindByteVector(stmt.get(), encryptMaterial.salt, 3);
  Sqlite3Utils::BindTimestamp(stmt.get(), now, 4);

  if (!Sqlite3Utils::TryExecuteStore(stmt.get())) {
    throw std::runtime_error("Failed to execute Store statement: " + std::string(sqlite3_errmsg(db_obs_)));
  }
}

std::optional<EncryptedKeyMaterial> Sqlite3KeyStore::Load() {
  constexpr auto sql = "SELECT algorithm, encrypted_key, salt FROM key_store WHERE id = 1;";

  sqlite3_stmt* raw_stmt = nullptr;

  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Failed to prepare Load statement: " + std::string(sqlite3_errmsg(db_obs_)));
  }

  const Sqlite3Utils::ScopedStmt stmt(raw_stmt);

  if (!Sqlite3Utils::TryExecuteLoad(stmt.get())) {
    return std::nullopt;
  }

  EncryptedKeyMaterial key_material;

  Sqlite3Utils::TryReadText(stmt.get(), key_material.algorithm, 0);
  Sqlite3Utils::TryReadBlob(stmt.get(), key_material.encrypted_key, 1);
  Sqlite3Utils::TryReadBlob(stmt.get(), key_material.salt, 2);

  return key_material;
}

void Sqlite3KeyStore::Clear() {
  constexpr auto sql = "DELETE FROM key_store WHERE id = 1;";

  if (char* err_msg = nullptr; !Sqlite3Utils::TryExecute(db_obs_, sql, err_msg)) {
    std::string err_str = err_msg
      ? err_msg
      : "Unknown error";

    if (err_msg) {
      sqlite3_free(err_msg);
    }

    throw std::runtime_error(std::format("Failed to clear KeyStore: {}", err_str));
  }

}

void Sqlite3KeyStore::InitSchema() const {
  constexpr auto sql = R"(
    CREATE TABLE IF NOT EXISTS key_store (
      id INTEGER PRIMARY KEY CHECK (id = 1),
      algorithm TEXT NOT NULL,
      encrypted_key BLOB NOT NULL,
      salt BLOB NOT NULL,
      stored_at INTEGER NOT NULL
    );
  )";

  if (char* err_msg = nullptr; !Sqlite3Utils::TryExecute(db_obs_, sql, err_msg)) {
    std::string err_str = err_msg 
      ? err_msg 
      : "Unknown error";

    if (err_msg) {
      sqlite3_free(err_msg);
    }

    throw std::runtime_error(std::format("Failed to initialize key_store schema: {}", err_str));
  }
}