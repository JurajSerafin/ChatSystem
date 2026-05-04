#ifndef PQXX_ROW_H
#define PQXX_ROW_H

#include <Database/i_row.h>

#include <chrono>
#include <string>

#include <pqxx/pqxx>

class PqxxRow : public IRow {
public:

  explicit PqxxRow(pqxx::row row) : row_(std::move(row)) {};

  [[nodiscard]] std::string GetString(const char* columnName) const override;

  [[nodiscard]] int GetInt(const char* columnName) const override;

  [[nodiscard]] std::string GetUuid(const char* columnName) const override;

  [[nodiscard]] std::chrono::system_clock::time_point GetTimeStamp(const char* columnName) const override;

  [[nodiscard]] bool IsNull(const char* columnName) const override;

private:
  pqxx::row row_;

  [[nodiscard]] std::chrono::system_clock::time_point ParsePqxxTimeStamp(const char* columnName) const;
};


inline std::string PqxxRow::GetString(const char* columnName) const {
  return row_[columnName].as<std::string>();
}

inline int PqxxRow::GetInt(const char* columnName) const {
  return row_[columnName].as<int>();
}

inline std::string PqxxRow::GetUuid(const char* columnName) const {
  return GetString(columnName);
}

inline std::chrono::system_clock::time_point PqxxRow::GetTimeStamp(const char* columnName) const {
  return ParsePqxxTimeStamp(GetString(columnName));
}

inline bool PqxxRow::IsNull(const char* columnName) const {
  return row_[columnName].is_null();
}

#endif // PQXX_ROW_H