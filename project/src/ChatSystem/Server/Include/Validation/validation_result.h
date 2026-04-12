#ifndef VALIDATION_RESULT_H
#define VALIDATION_RESULT_H

#include <string>
#include <vector>
#include <sstream>

/**
 * @brief Represents a single validation error.
 */
struct ValidationError {
  std::string field;   ///< Name of the field that failed validation
  std::string message;  ///< Description of the validation error
};

/**
 * @brief Collects validation errors produced during validation.
 *
 * A ValidationResult stores a list of ValidationError objects and
 * provides helper methods to inspect and summarize validation state.
 */
class ValidationResult {
public:
  /**
   * @brief Adds a validation error to the result.
   *
   * @param field Name of the field that failed validation.
   * @param message Description of the error.
   */
  void AddError(std::string field, std::string message) {
    errors_.push_back({
      .field = std::move(field),
      .message = std::move(message)
      });
  }

  /**
   * @brief Checks whether the validation succeeded.
   *
   * @return true if no errors are present, false otherwise.
   */
  [[nodiscard]] bool IsValid() const;

  /**
   * @brief Returns all validation errors.
   *
   * @return Constant reference to the internal error list.
   */
  [[nodiscard]] const std::vector<ValidationError>& Errors() const;

  /**
   * @brief Returns a human-readable summary of all errors.
   *
   * Each error is formatted as:
   * [field] message
   *
   * @return String containing all validation errors.
   */
  [[nodiscard]] std::string Summary() const;

private:
  std::vector<ValidationError> errors_;
};

inline bool ValidationResult::IsValid() const {
  return errors_.empty();
}

inline const std::vector<ValidationError>& ValidationResult::Errors() const {
  return errors_;
}

inline std::string ValidationResult::Summary() const {
  std::stringstream error_stream;

  for (const auto& e : errors_) {
    error_stream << "[" << e.field << "] " << e.message << "\n";
  }

  return error_stream.str();
}

#endif  // VALIDATION_RESULT_H