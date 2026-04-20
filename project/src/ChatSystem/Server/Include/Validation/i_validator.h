#ifndef I_VALIDATOR_H
#define I_VALIDATOR_H


#include <Validation/Core/validation_result.h>


/**
 * @brief Generic interface for validators.
 *
 * This interface defines a contract for validators that inspect
 * a parameter object and return a ValidationResult containing
 * any validation errors found.
 *
 * @tparam TParams Type of the object being validated.
 * @tparam MaxErrors The maximum number of validation errors this validator can produce.
 */
template <typename TParams, std::size_t MaxErrors>
class IValidator {
protected:
  static constexpr std::size_t kMaxErrors = MaxErrors;
public:

  /**
   * @brief Validates the given parameters.
   *
   * Performs validation logic on the provided object and returns
   * a ValidationResult describing any validation errors.
   *
   * @param params Object to validate.
   * @return ValidationResult containing validation errors.
   */
  [[nodiscard]] constexpr virtual validation::ValidationResult<kMaxErrors> Validate(const TParams& params) const = 0;

  /**
   * @brief Virtual destructor.
   *
   * Ensures proper cleanup of derived validator implementations.
   */
  virtual ~IValidator() = default;
};



#endif  // I_VALIDATOR_H