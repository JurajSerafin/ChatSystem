#include "sqlite3_utils.h"

#include "Database/local_db_models.h"

namespace Sqlite3Utils {

  bool TryPrepare(sqlite3* dbObs, const char* sql, sqlite3_stmt** rawStmt) {
    return sqlite3_prepare_v2(dbObs, sql, -1, rawStmt, nullptr) == SQLITE_OK;
  }

  bool TryExecute(sqlite3* dbObs, const char* sql, char*& errMsgOut) {
    return sqlite3_exec(dbObs, sql, nullptr, nullptr, &errMsgOut) == SQLITE_OK;
  }

  bool TryExecuteStore(sqlite3_stmt* rawStmt) {
    return sqlite3_step(rawStmt) == SQLITE_DONE;
  }

  bool TryExecuteLoad(sqlite3_stmt* rawStmt) {
    return sqlite3_step(rawStmt) == SQLITE_ROW;
  }

  void TryReadText(sqlite3_stmt* rawStmt, std::string& textOut, int col) {
    if (const auto text_ptr_out = sqlite3_column_text(rawStmt, col)) {
      textOut = reinterpret_cast<const char*>(text_ptr_out);
    }
  }

  void TryReadBlob(sqlite3_stmt* rawStmt, std::vector<unsigned char>& vecBlobOut, int col) {
    const auto blob_ptr = sqlite3_column_blob(rawStmt, col);

    const int blob_bytes = sqlite3_column_bytes(rawStmt, col);

    if (blob_ptr && blob_bytes > 0) {
      const auto* typed_ptr = static_cast<const unsigned char*>(blob_ptr);

      vecBlobOut.assign(typed_ptr, typed_ptr + blob_bytes);
    }
  }

  void TryReadTimeStamp(sqlite3_stmt* rawStmt, std::chrono::system_clock::time_point& timestampOut, int col) {
    if (sqlite3_column_type(rawStmt, col) != SQLITE_NULL) {

      int64_t ts_millis = sqlite3_column_int64(rawStmt, col);

      timestampOut = FromEpochMs(ts_millis);
    }
  }

  void TryReadOptionalText(sqlite3_stmt* rawStmt, std::optional<std::string>& optOut, int col) {
    if (sqlite3_column_type(rawStmt, col) == SQLITE_NULL) {
      optOut = std::nullopt;
      return;
    }

    if (const auto text_ptr = sqlite3_column_text(rawStmt, col)) {
      optOut = reinterpret_cast<const char*>(text_ptr);
    }
    else {
      optOut = std::nullopt;
    }
  }

  int64_t ToEpochMs(std::chrono::system_clock::time_point tp) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
  }

  std::chrono::system_clock::time_point FromEpochMs(int64_t ms) {
    return std::chrono::system_clock::time_point{ std::chrono::milliseconds{ms} };
  }

  void InitSchemaOrThrow(sqlite3* dbObs, const char* sql) {
    if (char* err_msg = nullptr; !TryExecute(dbObs, sql, err_msg)) {
      std::string err_str =
        err_msg
        ? err_msg : "Unknown error";

      if (err_msg) {
        sqlite3_free(err_msg);
      }
      throw std::runtime_error(std::format("Failed to initialize database schema: {}", err_str));
    }
  }

  void BindOptional(sqlite3_stmt* rawStmt, const std::optional<std::string>& optText, int col) {
    if (optText.has_value()) {
      sqlite3_bind_text(rawStmt, col, optText->c_str(), -1, SQLITE_TRANSIENT);
    }
    else {
      sqlite3_bind_null(rawStmt, col);
    }
  }

  void BindStr(sqlite3_stmt* rawStmt, const std::string& str, int col) {
    sqlite3_bind_text(rawStmt, col, str.c_str(), static_cast<int>(str.size()), SQLITE_TRANSIENT);
  }

  void BindStr(sqlite3_stmt* rawStmt, std::string_view str, int col) {
    sqlite3_bind_text(rawStmt, col, str.data(), static_cast<int>(str.size()), SQLITE_TRANSIENT);
  }

  void BindULL(sqlite3_stmt* rawStmt, std::size_t val, int col) {
    sqlite3_bind_int64(rawStmt, col, static_cast<sqlite3_int64>(val));
  }

  void BindTimestamp(sqlite3_stmt* rawStmt, std::chrono::system_clock::time_point timestamp, int col) {
    sqlite3_bind_int64(rawStmt, col, ToEpochMs(timestamp));
  }

  void BindBool(sqlite3_stmt* rawStmt, bool val, int col) {
    sqlite3_bind_int(rawStmt, col, val ? 1 : 0);
  }


} // namespace Sqlite3Utils