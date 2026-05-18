#ifndef HTTP_EXCEPTIONS_H
#define HTTP_EXCEPTIONS_H

#include <stdexcept>

/**
 * @brief Exception thrown when the server returns an HTTP 401 Unauthorized status.
 * Indicates missing, invalid, or expired authentication credentials.
 */
class UnauthorizedException : public std::runtime_error {
public:
  UnauthorizedException() : std::runtime_error("401 Unauthorized") {}

  constexpr static unsigned StatusCode();
};

/**
 * @brief Exception thrown when the server returns an HTTP 404 Not Found status.
 * Indicates the requested resource does not exist.
 */
class NotFoundException : public std::runtime_error {
public:
  explicit NotFoundException(const std::string& msg) : std::runtime_error("404 Not Found: " + msg) {}

  constexpr static unsigned StatusCode();
};

/**
 * @brief Exception thrown when the server returns an HTTP 422 (or 400) Validation Error.
 * Indicates the client sent a malformed request or invalid data parameters.
 */
class ValidationException : public std::runtime_error {
public:
  explicit ValidationException(const std::string& msg) : std::runtime_error("422 Validation Error: " + msg) {}

  constexpr static unsigned StatusCode();
};

/**
 * @brief Exception thrown when the server returns an HTTP 500 Internal Server Error status.
 * Indicates an unexpected failure on the server.
 */
class ServerException : public std::runtime_error {
public:
  explicit ServerException(const std::string& msg) : std::runtime_error("500 Server Error: " + msg) {}

  constexpr static unsigned StatusCode();
};

constexpr unsigned UnauthorizedException::StatusCode() {
  return 401;
}

constexpr unsigned NotFoundException::StatusCode() {
  return 404;
}

constexpr unsigned ValidationException::StatusCode() {
  return 422;
}

constexpr unsigned ServerException::StatusCode() {
  return 500;
}

#endif // HTTP_EXCEPTIONS_H