#ifndef I_ROW_H
#define I_ROW_H

#include <chrono>

class IRow {
public:
  [[nodiscard]] virtual std::string GetString(const char* columnName) const;

  [[nodiscard]] virtual int GetInt(const char* columnName) const;

  [[nodiscard]] virtual std::string GetUuid(const char* columnName) const;

  [[nodiscard]] virtual std::chrono::system_clock::time_point GetTimeStamp(const char* columnName) const;

  [[nodiscard]] virtual bool IsNull(const char* columnName) const;
}; 

#endif // I_ROW_H