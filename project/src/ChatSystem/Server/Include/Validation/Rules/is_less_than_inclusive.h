#ifndef IS_LESS_THAN_INCLUSIVE_H
#define IS_LESS_THAN_INCLUSIVE_H

#include <Validation/Rules/rule_base.h>
#include <Validation/Concepts/rule_concepts.h>
#include <Validation/Core/validation_result.h>

#include <utility>

namespace validation {

  namespace rules {

    /**
     * @brief Rule that validates a value is less than or equal to a threshold.
     * This rule compares an incoming value against a stored threshold using the
     * `<=` operator.
     * @tparam TThreshold The type of the threshold.
     */
    template<typename TThreshold>
    struct IsLessThanInclusive_t : RuleBase<1> {

      /// The maximum allowed value(one we compare against).
      const TThreshold threshold;

      /**
       * @brief Constructs the rule using perfect forwarding.
       * 
       * @tparam UThreshold The forwarded type of the threshold.
       * @param th The threshold value to store or bind to.
       */
      template <typename UThreshold>
      constexpr explicit IsLessThanInclusive_t(UThreshold&& th) : threshold(std::forward<UThreshold>(th)) {}

      /**
       * @brief Evaluates if the value is less than or equal to the threshold.
       * 
       * @tparam TValue The type of the value being validated.
       * @param thisVal The value to check.
       * @return A ValidationResult containing an error if the validation fails.
       */
      template <typename TValue>
        requires HasLessThanInclusive<TValue, TThreshold>
      constexpr auto operator()(const TValue& thisVal) const noexcept;
    };

    template<typename TThreshold>
    template <typename TValue>
      requires HasLessThanInclusive<TValue, TThreshold>
    constexpr auto IsLessThanInclusive_t<TThreshold>::operator()(const TValue& thisVal) const noexcept {
      ValidationResult<capacity> result;

      if (!(thisVal <= threshold)) {
        result.AddError({ "", "Must be less than or equal to the threshold" });
      }

      return result;
    }

    /**
     * @brief Helper function to instantiate the IsLessThanInclusive rule.
     *
     * @warning Because this natively binds references for lvalues, the returned rule
     * should only be used transiently (e.g., inside a Validate function) and never
     * stored or returned to a broader scope.
     * 
     * @tparam TThreshold The deduced type of the threshold.
     * @param threshold The maximum allowed value(one we compare against).
     * @return An IsLessThanInclusive_t rule instance.
     */
    template<typename TThreshold>
    constexpr auto IsLessThanInclusive(TThreshold&& threshold) {
      return IsLessThanInclusive_t<TThreshold>{std::forward<TThreshold>(threshold)};
    }

  } // namespace validation::rules

} // namespace validation

#endif // IS_LESS_THAN_INCLUSIVE_H