#ifndef BOUND_RULE_H
#define BOUND_RULE_H

#include "and_rule.h"

#include <Validation/Concepts/rule_concepts.h>
#include <Validation/Rules/rule_base.h>
#include <string_view>
#include <utility>

namespace validation {
  namespace rules {

    /**
     * @brief An executable rule bound to a specific struct/class member.
     * 
     * When executed, it extracts the target member from the parent object,
     * passes it to the underlying rule, and injects the field name into any
     * validation errors that are generated.
     * 
     * @tparam Object The type of the parent object.
     * @tparam Member The type of the member being validated.
     * @tparam TRule The underlying rule to execute against the member.
     */
    template<typename Object, typename Member, HasCapacity TRule>
    struct BoundRule : RuleBase<TRule::capacity> {

      /// Pointer to the target member.
      Member Object::* member;

      /// The string name of the field, injected into errors.
      std::string_view field;

      /// The validation rule to apply.
      TRule rule;

      /**
       * @brief Constructs a BoundRule.
       * 
       * @param memb Pointer to the member in the object.
       * @param fd The string representation of the field name.
       * @param rl The underlying validation rule to evaluate.
       */
      constexpr explicit BoundRule(Member Object::* memb, std::string_view fd, TRule rl)
        : member(memb), field(fd), rule(std::move(rl)) {
      }

      /**
       * @brief Executes the validation against an instance of the parent object.
       * 
       * @param object The object whose member needs validation.
       * @return A ValidationResult containing any errors, with the field name populated.
       */
      constexpr auto operator()(const Object& object) const requires RuleFor<TRule, Member>;
    };

    template <typename Object, typename Member, HasCapacity TRule>
    constexpr auto BoundRule<Object, Member, TRule>::operator()(const Object& object) const
      requires RuleFor<TRule, Member> {

      auto result = rule(object.*member);

      for (std::size_t error_i = 0; error_i < result.count; ++error_i) {
        result.errors[error_i].field = field;
      }

      return result;
    }
  } // namespace validation::rules
} // namespace validation

#endif // BOUND_RULE_H