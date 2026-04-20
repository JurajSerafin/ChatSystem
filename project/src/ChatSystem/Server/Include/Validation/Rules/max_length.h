#ifndef MAX_LENGTH_H
#define MAX_LENGTH_H

#include <Validation/Core/validation_result.h>
#include <Validation/Rules/rule_base.h>
#include <Validation/utils.h>
#include <ranges>

namespace validation {

  namespace rules {

    /**
     * @brief Rule that validates a range does not exceed a maximum length.
     * @tparam Max The maximum allowed length (inclusive).
     */
    template<std::size_t Max>
    struct MaxLength_t : RuleBase<1> {

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

    template <std::size_t Max>
    template <std::ranges::range TRange>
    constexpr auto MaxLength_t<Max>::operator()(const TRange& rangeVal) const noexcept {
      ValidationResult<1> result;

      if (GetLength(rangeVal) > Max) {
        result.AddError({ "", "Is too long" });
      }

      return result;
    }

    /// Global variable template for the MaxLength rule for simple compositional usage.
    template <std::size_t Max>
    inline constexpr MaxLength_t<Max> MaxLength{};
  } // namespace validation::rules

} // namespace validation

#endif // MAX_LENGTH_H