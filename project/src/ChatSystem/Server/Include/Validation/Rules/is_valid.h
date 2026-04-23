#ifndef IS_VALID_H
#define IS_VALID_H

#include <Validation/Rules/rule_base.h>
#include <Validation/Concepts/rule_concepts.h>
#include <Validation/Core/validation_result.h>

namespace validation {

  namespace rules {

    /**
     * @brief Rule that delegates validation to an object's IsValid() method.
     */
    struct IsValid_t : RuleBase<1> {

      /**
       * @brief Evaluates the object's validity.
       * 
       * @tparam TValidatable A type that satisfies the HasIsValid concept.
       * @param validatable The object to validate.
       * @return A ValidationResult indicating success or containing an invalidity error.
       */
      template<HasIsValid TValidatable>
      constexpr auto operator()(const TValidatable& validatable) const noexcept;
    };

    template <HasIsValid TValidatable>
    constexpr auto IsValid_t::operator()(const TValidatable& validatable) const noexcept {
      ValidationResult<capacity> result;

      if (!validatable.IsValid()) {
        result.AddError({ "", "Has to be valid" });
      }

      return result;
    }

    /// Global instance of the IsValid rule for simple compositional usage.
    inline constexpr IsValid_t IsValid{};
  } // namespace validation::rules

} // namespace validation

#endif // IS_VALID_H