#ifndef I_ROW_H
#define I_ROW_H

#include <chrono>
#include <string>

/**
 * @brief Interface for extracting typed data from a single database row.
 */
class IRow {
public:
  virtual ~IRow() = default;

  /** @brief Extracts a standard string from the specified column. */
  [[nodiscard]] virtual std::string GetString(const char* columnName) const = 0;

  /** @brief Extracts an integer from the specified column. */
  [[nodiscard]] virtual int GetInt(const char* columnName) const = 0;

  /** @brief Extracts a UUID string from the specified column. */
  [[nodiscard]] virtual std::string GetUuid(const char* columnName) const = 0;

  /** @brief Extracts a timestamp and maps it to a C++ system_clock time_point. */
  [[nodiscard]] virtual std::chrono::system_clock::time_point GetTimeStamp(const char* columnName) const = 0;

  /** @brief Checks if the specified column evaluates to SQL NULL. */
  [[nodiscard]] virtual bool IsNull(const char* columnName) const = 0;
};

#endif // I_ROW_H