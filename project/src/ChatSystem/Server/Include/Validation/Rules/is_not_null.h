#ifndef IS_NOT_NULL_H
#define IS_NOT_NULL_H

#include <Validation/Rules/rule_base.h>
#include <Validation/Concepts/rule_concepts.h>
#include <Validation/Core/validation_result.h>

namespace validation {

  namespace rules {

    /**
     * @brief Rule that validates a pointer-like object is not null.
     * 
     * Works with raw pointers, std::unique_ptr, std::shared_ptr.
     */
    struct IsNotNull_t : RuleBase<1> {

      /**
       * @brief Evaluates the pointer.
       * 
       * @tparam TPointer A type that satisfies the NullablePointer concept.
       * @param ptr The pointer to evaluate.
       * @return A ValidationResult indicating success or containing a null error.
       */
      template<NullablePointer TPointer>
      constexpr auto operator()(const TPointer& ptr) const noexcept;
    };

    template <NullablePointer TPointer>
    constexpr auto IsNotNull_t::operator()(const TPointer& ptr) const noexcept {
      ValidationResult<1> result;

      if (!ptr) {
        result.AddError({ "", "Must not be null" });
      }

      return result;
    }

    /// Global instance of the IsNotNull rule for simple compositional usage.
    inline constexpr IsNotNull_t IsNotNull{};
  }

}

#endif // IS_NOT_NULL_H