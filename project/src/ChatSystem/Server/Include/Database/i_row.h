#ifndef I_ROW_H
#define I_ROW_H

#include <string>
#include <chrono>

class IRow {
public:
  [[nodiscard]] virtual const std::string& GetString() const;

  [[nodiscard]] virtual int GetInt() const;

  [[nodiscard]] virtual const std::string& GetUuid() const;

  [[nodiscard]] virtual const std::chrono::system_clock::time_point & GetTimeStamp() const;

  [[nodiscard]] virtual bool IsNull() const;
}; 

#endif // I_ROW_H