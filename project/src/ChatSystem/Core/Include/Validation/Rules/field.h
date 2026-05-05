#ifndef FIELD_H
#define FIELD_H

#include <string_view>

#include <Validation/Concepts/rule_concepts.h>
#include <Validation/Rules/bound_rule.h>

namespace validation {
  namespace rules {

    /**
     * @brief Represents a struct/class member targeted for validation.
     * 
     * Acts as an intermediate binder. It holds a pointer to a class member
     * and the string representation of that member's name. It is typically
     * instantiated via the BIND_FIELD macro.
     * 
     * @tparam Object The class or struct type containing the member.
     * @tparam Member The type of the member being bound.
     */
    template <typename Object, typename Member>
    struct Field {

      /// Pointer to the member within the object.
      Member Object::* member;

      /// The human-readable string name of the member.
      std::string_view name;

      /**
       * @brief Constructs a Field binder.
       * 
       * @param mem Pointer to the member.
       * @param nm String literal representing the member's name.
       */
      constexpr Field(Member Object::* mem, const std::string_view nm) : member(mem), name(nm) {}

      /**
       * @brief Binds this field to a specific validation rule.
       * 
       * Allows the declarative `BIND_FIELD(Obj, member) | Rule` syntax.
       * 
       * @tparam TRule The rule to apply to this field.
       * @param rule The instance of the validation rule.
       * @return A BoundRule that can be executed against an instance of the Object.
       */
      template<HasCapacity TRule>
      constexpr auto operator|(TRule rule) const {
        return BoundRule<Object, Member, TRule>{member, name, std::move(rule)};
      }
    };
  }
}

#endif // FIELD_H