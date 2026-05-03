#ifndef I_ROW_H
#define I_ROW_H

#include <string>
#include <chrono>

class IRow {
public:
  [[nodiscard]] virtual std::string GetString(const std::string& columnName) const;

  [[nodiscard]] virtual int GetInt(const std::string& columnName) const;

  [[nodiscard]] virtual std::string GetUuid(const std::string& columnName) const;

  [[nodiscard]] virtual std::chrono::system_clock::time_point GetTimeStamp(const std::string& columnName) const;

  [[nodiscard]] virtual bool IsNull(const std::string& columnName) const;
}; 

#endif // I_ROW_H