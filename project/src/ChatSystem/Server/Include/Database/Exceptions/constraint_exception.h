#ifndef CONSTRAINT_EXCEPTION_H
#define CONSTRAINT_EXCEPTION_H

#include <stdexcept>
#include <string>


class ConstraintException : public std::runtime_error {
public:

	explicit ConstraintException(const std::string& message) : std::runtime_error(message) {}

	explicit ConstraintException(const char* message) : std::runtime_error(message) {}
};

#endif // CONSTRAINT_EXCEPTION_H