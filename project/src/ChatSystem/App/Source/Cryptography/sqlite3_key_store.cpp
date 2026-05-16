#include "sqlite3_key_store.h"
#include "Cryptography/encrypted_key_material.h"

#include <memory>
#include <utility>
#include <chrono>
#include <optional>

#include <format>

namespace {
  using ScopedStmt = std::unique_ptr <sqlite3_stmt, decltype([](sqlite3_stmt* stmt) {
    if (stmt) {
      sqlite3_finalize(stmt);
    }
  })> ;
} // namespace

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

Sqlite3KeyStore::Sqlite3KeyStore(std::string_view dbPath) {
  if (sqlite3_open(dbPath.data(), &db_obs_) != SQLITE_OK) {
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

bool Sqlite3KeyStore::TryPrepare(const char* sql, sqlite3_stmt** rawStmt) const {
  return sqlite3_prepare_v2(db_obs_, sql, -1, rawStmt, nullptr) == SQLITE_OK;
}

void Sqlite3KeyStore::TryReadText(sqlite3_stmt* rawStmt, std::string& textOut, int col) {
  if (const auto text_ptr_out = sqlite3_column_text(rawStmt, col)) {
    textOut = reinterpret_cast<const char*>(text_ptr_out);
  }
}

void Sqlite3KeyStore::TryReadBlob(sqlite3_stmt* rawStmt, std::vector<unsigned char>& vecBlobOut, int col) {
  const auto blob_ptr = sqlite3_column_blob(rawStmt, col);
  
  const int blob_bytes = sqlite3_column_bytes(rawStmt, col);

  if (blob_ptr) {
    const auto* typed_ptr = static_cast<const unsigned char*>(blob_ptr);

    vecBlobOut.assign(typed_ptr, typed_ptr + blob_bytes);
  }
}

bool Sqlite3KeyStore::TryExecute(const char* sql, char*& errMsgOut) const {

  return sqlite3_exec(db_obs_, sql, nullptr, nullptr, &errMsgOut) == SQLITE_OK;
}

bool Sqlite3KeyStore::TryExecuteStore(sqlite3_stmt* rawStmt) {
  return sqlite3_step(rawStmt) == SQLITE_DONE;
}

bool Sqlite3KeyStore::TryExecuteLoad(sqlite3_stmt* rawStmt) {
  return sqlite3_step(rawStmt) == SQLITE_ROW;
}

void Sqlite3KeyStore::Store(const EncryptedKeyMaterial& encryptMaterial) {
  constexpr auto sql = R"(
    INSERT OR REPLACE INTO key_store (id, algorithm, encrypted_key, iv, salt, stored_at) 
    VALUES (1, ?, ?, ?, ?, ?);
  )";

  sqlite3_stmt* raw_stmt = nullptr;

  if (!TryPrepare(sql, &raw_stmt)) {
    throw std::runtime_error("Failed to prepare Store statement: " + std::string(sqlite3_errmsg(db_obs_)));
  }

  const ScopedStmt stmt { raw_stmt };

  const auto now = std::chrono::system_clock::now();
  const int64_t now_timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

  sqlite3_bind_text(stmt.get(), 1, encryptMaterial.algorithm.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_blob(stmt.get(), 2, encryptMaterial.encrypted_key.data(), static_cast<int>(encryptMaterial.encrypted_key.size()), SQLITE_TRANSIENT);
  sqlite3_bind_blob(stmt.get(), 3, encryptMaterial.iv.data(), static_cast<int>(encryptMaterial.iv.size()), SQLITE_TRANSIENT);
  sqlite3_bind_blob(stmt.get(), 4, encryptMaterial.salt.data(), static_cast<int>(encryptMaterial.salt.size()), SQLITE_TRANSIENT);
  sqlite3_bind_int64(stmt.get(), 5, now_timestamp);

  if (!TryExecuteStore(stmt.get())) {
    throw std::runtime_error("Failed to execute Store statement: " + std::string(sqlite3_errmsg(db_obs_)));
  }
}

std::optional<EncryptedKeyMaterial> Sqlite3KeyStore::Load() {
  constexpr auto sql = "SELECT algorithm, encrypted_key, iv, salt FROM key_store WHERE id = 1;";

  sqlite3_stmt* raw_stmt = nullptr;

  if (!TryPrepare(sql, &raw_stmt)) {
    throw std::runtime_error("Failed to prepare Load statement: " + std::string(sqlite3_errmsg(db_obs_)));
  }

  const ScopedStmt stmt(raw_stmt);

  if (!TryExecuteLoad(stmt.get())) {
    return std::nullopt;
  }

  EncryptedKeyMaterial key_material;

  TryReadText(stmt.get(), key_material.algorithm, 0);
  TryReadBlob(stmt.get(), key_material.encrypted_key, 1);
  TryReadBlob(stmt.get(), key_material.iv, 2);
  TryReadBlob(stmt.get(), key_material.salt, 3);

  return key_material;
}

void Sqlite3KeyStore::Clear() {
  constexpr auto sql = "DELETE FROM key_store WHERE id = 1;";

  if (char* err_msg = nullptr; !TryExecute(sql, err_msg)) {
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
      iv BLOB NOT NULL,
      salt BLOB NOT NULL,
      stored_at INTEGER NOT NULL
    );
  )";

  if (char* err_msg = nullptr; !TryExecute(sql, err_msg)) {
    std::string err_str = err_msg 
      ? err_msg 
      : "Unknown error";

    if (err_msg) {
      sqlite3_free(err_msg);
    }

    throw std::runtime_error(std::format("Failed to initialize key_store schema: {}", err_str));
  }
}