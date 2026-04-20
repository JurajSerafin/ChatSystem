#ifndef VALIDATION_RESULT_H
#define VALIDATION_RESULT_H

#include "validation_error.h"

#include <array>
#include <string_view>
#include <utility>

namespace validation {

  /**
   * @brief A fixed-capacity container for accumulating validation errors.
   * 
   * Uses a statically sized std::array to avoid heap allocations during
   * validation. The maximum number of errors is strictly bounded at compile time
   * by the capacity of the rules being executed.
   * 
   * @tparam MaxErrors The maximum number of errors this result can hold.
   */
  template <std::size_t MaxErrors>
  struct ValidationResult {

    /// Statically allocated array holding the accumulated validation errors.
    std::array<ValidationError, MaxErrors> errors{};

    /// The current number of validation errors recorded.
    std::size_t count{ 0 };

    /**
     * @brief Checks if the validation was successful.
     * 
     * @return true if no errors were recorded (count is 0), false otherwise.
     */
    [[nodiscard]] constexpr bool Ok() const noexcept;

    /**
     * @brief Constructs and adds a new validation error.
     * If the internal capacity (MaxErrors) has been reached, the error is safely ignored.
     * 
     * @param field The name of the field that failed validation.
     * @param message A descriptive message explaining the failure.
     */
    constexpr void AddError(std::string_view field, std::string_view message) noexcept;

    /**
     * @brief Copies an existing validation error into the result.
     * If the internal capacity (MaxErrors) has been reached, the error is safely ignored.
     * 
     * @param error The ValidationError object to copy.
     */
    constexpr void AddError(const ValidationError& error) noexcept;

    /**
     * @brief Moves an existing validation error into the result.
     * Optimizes performance by transferring ownership of internal strings/data,
     * avoiding deep copies. If the internal capacity has been reached, the error is safely ignored.
     * 
     * @param error The ValidationError object to move.
     */
    constexpr void AddError(ValidationError&& error) noexcept;
  };

  template <std::size_t MaxErrors>
  constexpr bool ValidationResult<MaxErrors>::Ok() const noexcept {
    return count == 0;
  }

  template <std::size_t MaxErrors>
  constexpr void ValidationResult<MaxErrors>::AddError(std::string_view field,
    std::string_view message) noexcept {
    if (count < MaxErrors) {
      errors[count++] = ValidationError(field, message);
    }
  }

  template <std::size_t MaxErrors>
  constexpr void ValidationResult<MaxErrors>::AddError(const ValidationError& error) noexcept {
    if (count < MaxErrors) {
      errors[count++] = error;
    }
  }

  template <std::size_t MaxErrors>
  constexpr void ValidationResult<MaxErrors>::AddError(ValidationError&& error) noexcept {
    if (count < MaxErrors) {
      errors[count++] = std::move(error);
    }
  }
}  // namespace validation

#endif  // VALIDATION_RESULT_H