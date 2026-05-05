#ifndef UTILS_H
#define UTILS_H

#include <ranges>
#include <stdexcept>
#include <utility>
#include <string_view>
#include <format>
#include <Validation/Concepts/rule_concepts.h>

namespace validation {

  /**
   * @brief Safely computes the length or distance of a C++ range.
   * Optimizes for sized ranges (e.g., std::vector, std::string) using std::ranges::size,
   * while falling back to std::ranges::distance for unsized ranges.
   * 
   * @tparam TRange The type of the range.
   * @param rangeVal The range to measure.
   * @return std::size_t The number of elements in the range.
   */
  template<std::ranges::range TRange>
  constexpr std::size_t GetLength(const TRange& rangeVal) {
    if constexpr (std::ranges::sized_range<TRange>) {
      return std::ranges::size(rangeVal);
    }
    else {
      return std::ranges::distance(rangeVal);
    }
  }

  /**
   * @brief Generic helper to validate and assign a value to a class member.
   * Enforces validation rules before mutation.
   * 
   * @tparam TObject The type of the object being mutated.
   * @tparam TMember The type of the class member.
   * @tparam TValue The type of the incoming value.
   * @tparam TRule The validation rule to execute.
   * @param object The object instance to mutate.
   * @param member_ptr Pointer to the member variable to mutate.
   * @param value The value to validate and assign.
   * @param rule The rule to execute against the value.
   * @param errorPrefix The prefix for the exception message if validation fails.
   * @throws std::invalid_argument if the value fails validation.
   */
  template<typename TObject, typename TMember, typename TValue, HasCapacity TRule>
    requires RuleFor<TRule, TValue>
  constexpr void SetOrThrow(TObject& object, TMember TObject::* member_ptr, TValue&& value, const TRule& rule, std::string_view errorPrefix) {
    if (const auto validation = rule(std::forward<TValue>(value)); !validation.Ok()) {
      throw std::invalid_argument{ std::format("{} - {}\n", errorPrefix, validation.Summary()) };
    }

    object.*member_ptr = std::move(value);
  }

} // namespace validation

#endif // UTILS_H