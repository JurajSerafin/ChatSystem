#ifndef TRANSACTION_EXCEPTION_H
#define TRANSACTION_EXCEPTION_H

#include <stdexcept>
#include <string>


class TransactionException : public std::runtime_error {
public:

  explicit TransactionException(const std::string& message) : std::runtime_error(message) {}

  explicit TransactionException(const char* message) : std::runtime_error(message) {}
};

#endif // TRANSACTION_EXCEPTION_H