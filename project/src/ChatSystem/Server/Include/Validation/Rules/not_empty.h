#ifndef NOT_EMPTY_H
#define NOT_EMPTY_H

#include <ranges>

#include <Validation/Rules/rule_base.h>
#include <Validation/Core/validation_result.h>

namespace validation {

  namespace rules {

    /**
     * @brief Rule that validates that a range is not empty.
     * Effectively calls std::ranges::empty on the target to ensure it contains elements.
     */
    struct NotEmpty_t : RuleBase<1> {

      /**
       * @brief Evaluates whether the range is empty.
       * 
       * @tparam TRange A type that satisfies the std::ranges::range concept.
       * @param rangeVal The range to evaluate.
       * @return A ValidationResult indicating success or containing an empty error.
       */
      template<std::ranges::range TRange>
      constexpr auto operator()(const TRange& rangeVal) const noexcept;
    };

    template <std::ranges::range TRange>
    constexpr auto NotEmpty_t::operator()(const TRange& rangeVal) const noexcept {
      ValidationResult<1> result;

      if (std::ranges::empty(rangeVal)) {
        result.AddError({ "", "Must not be empty" });
      }

      return result;
    }

    /// Global instance of the NotEmpty rule for simple compositional usage.
    inline constexpr NotEmpty_t NotEmpty{};
  } // namespace validation::rules

} // namespace validation

#endif // NOT_EMPTY_H