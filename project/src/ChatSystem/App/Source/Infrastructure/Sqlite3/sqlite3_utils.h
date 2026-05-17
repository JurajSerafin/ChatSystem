#ifndef SQLITE3_UTILS_H
#define SQLITE3_UTILS_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <chrono>
#include <cstdint>
#include <memory>

namespace Sqlite3Utils {

  struct StmtDeleter {
    void operator()(sqlite3_stmt* stmt) const {
      if (stmt) {
        sqlite3_finalize(stmt);
      }
    }
  };

  using ScopedStmt = std::unique_ptr<sqlite3_stmt, StmtDeleter>;

  bool TryPrepare(sqlite3* dbObs, const char* sql, sqlite3_stmt** rawStmt);
  bool TryExecute(sqlite3* dbObs, const char* sql, char*& errMsgOut);

  bool TryExecuteStore(sqlite3_stmt* rawStmt);
  bool TryExecuteLoad(sqlite3_stmt* rawStmt);

  void TryReadText(sqlite3_stmt* rawStmt, std::string& textOut, int col);
  void TryReadBlob(sqlite3_stmt* rawStmt, std::vector<unsigned char>& vecBlobOut, int col);
  void TryReadTimeStamp(sqlite3_stmt* rawStmt, std::chrono::system_clock::time_point& timestampOut, int col);
  void TryReadOptionalText(sqlite3_stmt* rawStmt, std::optional<std::string>& optOut, int col);
  void TryReadBool(sqlite3_stmt* rawStmt, bool& valOut, int col);

  int64_t ToEpochMs(std::chrono::system_clock::time_point tp);
  std::chrono::system_clock::time_point FromEpochMs(int64_t ms);

  void InitSchemaOrThrow(sqlite3* dbObs, const char* sql);

  void BindOptional(sqlite3_stmt* rawStmt, const std::optional<std::string>& optText, int col);

  void BindStr(sqlite3_stmt* rawStmt, const std::string& str, int col);

  void BindStr(sqlite3_stmt* rawStmt, std::string_view str, int col);

  void BindULL(sqlite3_stmt* rawStmt, std::size_t val, int col);

  void BindTimestamp(sqlite3_stmt* rawStmt, std::chrono::system_clock::time_point timestamp, int col);

  void BindBool(sqlite3_stmt* rawStmt, bool val, int col);

  void BindByteVector(sqlite3_stmt* rawStmt, const std::vector<unsigned char>& vec, int col);

} // namespace Sqlite3Utils

#endif // SQLITE3_UTILS_H