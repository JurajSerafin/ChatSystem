#ifndef CONNECTION_EXCEPTION_H
#define CONNECTION_EXCEPTION_H

#include <stdexcept>
#include <string>


class ConnectionException : public std::runtime_error {
public:

	explicit ConnectionException(const std::string& message) : std::runtime_error(message) {}

	explicit ConnectionException(const char* message) : std::runtime_error(message) {}
};

#endif // CONNECTION_EXCEPTION_H
