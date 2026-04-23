#ifndef TIME_POINT_MUST_BE_SET_H
#define TIME_POINT_MUST_BE_SET_H

#include <Validation/Core/validation_result.h>
#include <Validation/Rules/rule_base.h>
#include <chrono>

namespace validation {

  namespace rules {

    /**
     * @brief Rule that ensures a std::chrono::system_clock::time_point is not default-constructed.
     * 
     * This rule is used to verify that a timestamp has been explicitly assigned a value other
     * than the epoch.
     */
    struct TimePointMustBeSet_t : RuleBase<1> {

      /**
       * @brief Evaluates the time point.
       * 
       * @param tp The time point to check against the default-constructed value.
       * @return A ValidationResult indicating success or containing an uninitialized error.
       */
      constexpr auto operator()(std::chrono::system_clock::time_point tp) const noexcept;
    };

    constexpr auto TimePointMustBeSet_t::operator()(std::chrono::system_clock::time_point tp) const noexcept {
      ValidationResult<capacity> result;

      if (tp == std::chrono::system_clock::time_point{}) {
        result.AddError({ "", "Has to be set to a concrete time point" });
      }

      return result;
    }

    /// Global instance of the TimePointMustBeSet rule for simple compositional usage.
    inline constexpr TimePointMustBeSet_t TimePointMustBeSet{};
  } // namespace validation::rules

} // namespace validation

#endif // TIME_POINT_MUST_BE_SET_H