#ifndef VALIDATION_ERROR_H
#define VALIDATION_ERROR_H

#include <string>
#include <string_view>
#include <ostream>

namespace validation {

  /**
   * @brief Represents a single validation failure.
   * 
   * Contains the name of the field that failed validation and a human-readable
   * description of why it failed.
   */
  struct ValidationError {

    /// The name of the property or field that failed validation.
    std::string field;

    /// A descriptive message explaining the validation failure.
    std::string_view message;

    constexpr ValidationError() = default;

    /**
     * @brief Constructs a new ValidationError.
     * @param f The field name.
     * @param m The error message.
     */
    constexpr ValidationError(std::string_view f, std::string_view m) : field{ f }, message{ m } {}
  };

  /**
   * @brief Stream insertion operator for ValidationError.
   * Formats the error into a readable string for logging or debugging.
   * 
   * @param outputStream The output stream.
   * @param validationError The error to format.
   * @return std::ostream& The modified output stream.
   */
  inline std::ostream& operator<<(std::ostream& outputStream, const ValidationError& validationError) {
    return outputStream << "Field: '" << validationError.field << "', Error Message: " << validationError.message;
  }
}

#endif  // VALIDATION_ERROR_H