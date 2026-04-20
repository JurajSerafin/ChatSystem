#ifndef MIN_LENGTH_H
#define MIN_LENGTH_H

#include <ranges>
#include <cstddef>

#include <Validation/Rules/rule_base.h>
#include <Validation/Core/validation_result.h>
#include <Validation/utils.h>

namespace validation {

  namespace rules {

    /**
     * @brief Rule that validates a range meets a minimum length requirement.
     * 
     * @tparam Min The minimum allowed length (inclusive).
     */
    template<std::size_t Min>
    struct MinLength_t : RuleBase<1> {

      /**
       * @brief Evaluates the length of the provided range.
       * 
       * @tparam TRange A type that satisfies the std::ranges::range concept.
       * @param rangeVal The range to measure.
       * @return A ValidationResult indicating success or containing a length error.
       */
      template<std::ranges::range TRange>
      constexpr auto operator()(const TRange& rangeVal) const noexcept;
    };

    template <std::size_t Min>
    template <std::ranges::range TRange>
    constexpr auto MinLength_t<Min>::operator()(const TRange& rangeVal) const noexcept {
      ValidationResult<1> result;

      if (GetLength(rangeVal) < Min) {
        result.AddError({ "", "Is too short" });
      }

      return result;
    }

    /// Global variable template for the MinLength rule for simple compositional usage.
    template <std::size_t Min>
    inline constexpr MinLength_t<Min> MinLength{};
  } // namespace validation::rules

} // namespace validation

#endif // MIN_LENGTH_H