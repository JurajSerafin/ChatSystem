#include "pqxx_row.h"
#include "Database/Exceptions/column_parse_exception.h"

#include <chrono>
#include <format>
#include <sstream>

std::chrono::system_clock::time_point PqxxRow::ParsePqxxTimeStamp(const std::string& dbTimeStr) const {
  constexpr auto kTimeFormat = "%F %T";

  std::chrono::system_clock::time_point result_time_point;

  std::istringstream db_time_stream{ dbTimeStr };

  db_time_stream >> std::chrono::parse(kTimeFormat, result_time_point);

  if (db_time_stream.fail()) {
    throw ColumnParseException{ std::format("Unable to parse time stamp '{}'.", dbTimeStr) };
  }

  return result_time_point;
}