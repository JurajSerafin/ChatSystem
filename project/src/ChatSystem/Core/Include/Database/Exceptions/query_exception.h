#ifndef QUERY_EXCEPTION_H
#define QUERY_EXCEPTION_H

#include <stdexcept>
#include <string>

class QueryException : public std::runtime_error {
public:

  explicit QueryException(const std::string& message) : std::runtime_error(message) {}

  explicit QueryException(const char* message) : std::runtime_error(message) {}
};

#endif // QUERY_EXCEPTION_H