#ifndef COLUMN_PARSE_EXCEPTION_H
#define COLUMN_PARSE_EXCEPTION_H

#include <stdexcept>
#include <string>


class ColumnParseException : public std::runtime_error {
public:

  explicit ColumnParseException(const std::string& message) : std::runtime_error(message) {}

  explicit ColumnParseException(const char* message) : std::runtime_error(message) {}
};

#endif // COLUMN_PARSE_EXCEPTION_H