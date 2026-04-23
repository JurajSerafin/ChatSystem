#ifndef TRUE_RULE_H
#define TRUE_RULE_H

#include <Validation/Rules/rule_base.h>
#include <Validation/Core/validation_result.h>

namespace validation {

  namespace rules {

    /**
     * @brief Utility Rule that always evaluates as valid.
     * Useful as a placeholder or default rule for fields that do not currently
     * require any strict validation constraints.
     */
    struct TrueRule_t : RuleBase<0> {

      /**
       * @brief Evaluates the object's validity (always succeeds).
       *
       * @tparam TVal Any type.
       * @param val The object to validate.
       * @return A ValidationResult indicating success and containing no errors.
       */
      template<typename TVal>
      constexpr auto operator()(const TVal& val) const noexcept;
    };

    template<typename TVal>
    constexpr auto TrueRule_t::operator()(const TVal& val) const noexcept {
      return ValidationResult<capacity>{};
    }

    /// Global instance of the TrueRule rule for simple compositional usage.
    inline constexpr TrueRule_t TrueRule{};
  } // namespace validation::rules

} // namespace validation

#endif // TRUE_RULE_H