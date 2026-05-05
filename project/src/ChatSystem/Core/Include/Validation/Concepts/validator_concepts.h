#ifndef VALIDATOR_CONCEPTS_H
#define VALIDATOR_CONCEPTS_H

#include <concepts>
#include <type_traits>
#include <string>

namespace validation {

  /**
   * @brief Concept defining the base requirements for a generic validator.
   * Ensures that the validator type provides a `Validate` method taking the
   * specified parameters and returning a result object. The result object must
   * expose an `Ok()` boolean check and a `Summary()` string generator.
   * 
   * @tparam TValidator The validator type being checked.
   * @tparam TParams The data transfer object (DTO) type being validated.
   */
  template<typename TValidator, typename TParams>
  concept ValidatorFor = requires(TValidator validator, TParams params) {
    { validator.Validate(params).Ok() } -> std::convertible_to<bool>;
    { validator.Validate(params).Summary() } -> std::convertible_to<std::string>;
  };

} // namespace validation

#endif // VALIDATOR_CONCEPTS_H