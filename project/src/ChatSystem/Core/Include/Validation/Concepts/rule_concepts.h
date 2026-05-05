#ifndef RULE_CONCEPTS_H
#define RULE_CONCEPTS_H

#include <Validation/Core/validation_result.h>
#include <concepts>
#include <memory>
#include <type_traits>
#include <variant>

namespace validation {

  /**
   * @brief Concept ensuring a rule type defines a static compile-time capacity.
   * This is required to determine the maximum number of errors a rule can produce,
   * allowing the framework to use fixed-size arrays without heap allocation.
   * 
   * @tparam TRule The rule type to check.
   */
  template<typename TRule>
  concept HasCapacity = requires {
    { TRule::capacity } -> std::convertible_to<std::size_t>;
  };

  /**
   * @brief Concept ensuring a rule can validate a specific value type.
   * Validates that the rule exposes a capacity and provides an operator()
   * that takes a TValue and returns a ValidationResult bounded by that capacity.
   * 
   * @tparam TRule The rule type.
   * @tparam TValue The type of the value being validated.
   */
  template<typename TRule, typename TValue>
  concept RuleFor = HasCapacity<TRule> && requires(const TRule rule, const TValue value) {
    { rule(value) } -> std::convertible_to<ValidationResult<TRule::capacity>>;
  };

  /**
   * @brief Concept ensuring a type provides an IsValid() method.
   * 
   * @tparam TValidatable The type to check.
   */
  template<typename TValidatable>
  concept HasIsValidSingle = requires(const TValidatable instance) {
    { instance.IsValid() } -> std::convertible_to<bool>;
  };

  template<typename TValidatable>
  struct VariantMembersHaveIsValid : std::false_type{};

  template<typename ... TValidatable>
  struct VariantMembersHaveIsValid<std::variant<TValidatable...>> :
    std::bool_constant<(HasIsValidSingle<TValidatable> && ...)>{};

  template<typename TValidatable>
  concept HasIsValid = HasIsValidSingle<TValidatable> || VariantMembersHaveIsValid<TValidatable>::value;

  /**
   * @brief Concept ensuring a type behaves like a nullable pointer.
   * The type must be convertible to bool (to check for null)
   * and compatible with std::to_address.
   * 
   * @tparam TPtr The pointer-like type to check.
   */
  template<typename TPtr>
  concept NullablePointer = requires(TPtr ptr) {
    { static_cast<bool>(ptr) } -> std::convertible_to<bool>;
    { std::to_address(ptr) };
  };

  /**
   * @brief Concept ensuring a type supports the less-than-or-equal-to (<=) operator.
   * * Validates that an instance of the value type can be compared against a threshold
   * using `<=`, and that the result of this comparison can be evaluated as a boolean.
   * * @tparam TValue The type of the value being validated.
   * @tparam TThreshold The type of the threshold to compare against (defaults to TValue).
   */
  template<typename TValue, typename TThreshold = TValue>
  concept HasLessThanInclusive = requires(const TValue value, const TThreshold threshold) {
    { value <= threshold } -> std::convertible_to<bool>;
  };

}

#endif // RULE_CONCEPTS_H