#ifndef COLUMN_TYPE_MISMATCH_EXCEPTION_H
#define COLUMN_TYPE_MISMATCH_EXCEPTION_H

#include <stdexcept>
#include <string>


class ColumnTypeMismatchException : public std::runtime_error {
public:
  explicit ColumnTypeMismatchException(const std::string& message) : std::runtime_error(message) {}
  explicit ColumnTypeMismatchException(const char* message) : std::runtime_error(message) {}
};

#endif // COLUMN_TYPE_MISMATCH_EXCEPTION_H