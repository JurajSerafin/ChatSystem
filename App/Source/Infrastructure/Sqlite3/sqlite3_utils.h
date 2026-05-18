#ifndef SQLITE3_UTILS_H
#define SQLITE3_UTILS_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>

/**
 * @brief Low-level utility wrappers for the SQLite3 C API.
 * Provides RAII memory management, safe data binding, and row extraction helpers.
 */
namespace Sqlite3Utils {

  /** @brief Custom deleter ensuring SQLite prepared statements are properly finalized. */
  struct StmtDeleter {
    void operator()(sqlite3_stmt* stmt) const {
      if (stmt) {
        sqlite3_finalize(stmt);
      }
    }
  };

  /** @brief RAII wrapper for sqlite3_stmt to prevent memory leaks. */
  using ScopedStmt = std::unique_ptr<sqlite3_stmt, StmtDeleter>;

  /** @brief Compiles a SQL query into a prepared statement. Returns true on success. */
  bool TryPrepare(sqlite3* dbObs, const char* sql, sqlite3_stmt** rawStmt);

  /** @brief Executes a raw SQL query directly without preparing. Returns true on success. */
  bool TryExecute(sqlite3* dbObs, const char* sql, char*& errMsgOut);

  /** @brief Steps a statement expecting no returned rows (e.g., INSERT/UPDATE). Returns true on SQLITE_DONE. */
  bool TryExecuteStore(sqlite3_stmt* rawStmt);

  /** @brief Steps a statement expecting returned rows (e.g., SELECT). Returns true on SQLITE_ROW. */
  bool TryExecuteLoad(sqlite3_stmt* rawStmt);
 

  // Row Extraction Helpers

  /** @brief Extracts a standard string from the specified column index. */
  void TryReadText(sqlite3_stmt* rawStmt, std::string& textOut, int col);

  /** @brief Extracts a binary blob into a byte vector from the specified column index. */
  void TryReadBlob(sqlite3_stmt* rawStmt, std::vector<unsigned char>& vecBlobOut, int col);

  /** @brief Extracts an epoch timestamp and converts it to a C++ time_point. */
  void TryReadTimeStamp(sqlite3_stmt* rawStmt, std::chrono::system_clock::time_point& timestampOut, int col);

  /** @brief Extracts text, handling NULL values safely by returning std::nullopt. */
  void TryReadOptionalText(sqlite3_stmt* rawStmt, std::optional<std::string>& optOut, int col);

  /** @brief Extracts a boolean value from the specified column index. */
  void TryReadBool(sqlite3_stmt* rawStmt, bool& valOut, int col);


  // Time Conversion Utilities

  /** @brief Converts a C++ time_point to Unix epoch milliseconds. */
  int64_t ToEpochMs(std::chrono::system_clock::time_point tp);

  /** @brief Converts Unix epoch milliseconds back into a C++ time_point. */
  std::chrono::system_clock::time_point FromEpochMs(int64_t ms);


  // Initialization Helpers

  /** @brief Executes a DDL schema string and throws a runtime exception if it fails. */
  void InitSchemaOrThrow(sqlite3* dbObs, const char* sql);


  // Parameter Binding Helpers

  /** @brief Binds a string parameter, or NULL if the optional is empty. */
  void BindOptional(sqlite3_stmt* rawStmt, const std::optional<std::string>& optText, int col);

  /** @brief Binds a standard std::string to the statement. */
  void BindStr(sqlite3_stmt* rawStmt, const std::string& str, int col);

  /** @brief Binds a string_view to the statement. */
  void BindStr(sqlite3_stmt* rawStmt, std::string_view str, int col);

  /** @brief Binds an unsigned long long (std::size_t) to the statement. */
  void BindULL(sqlite3_stmt* rawStmt, std::size_t val, int col);

  /** @brief Binds a C++ time_point as epoch milliseconds to the statement. */
  void BindTimestamp(sqlite3_stmt* rawStmt, std::chrono::system_clock::time_point timestamp, int col);

  /** @brief Binds a boolean value to the statement. */
  void BindBool(sqlite3_stmt* rawStmt, bool val, int col);

  /** @brief Binds a vector of bytes as a binary BLOB to the statement. */
  void BindByteVector(sqlite3_stmt* rawStmt, const std::vector<unsigned char>& vec, int col);

} // namespace Sqlite3Utils

#endif // SQLITE3_UTILS_H