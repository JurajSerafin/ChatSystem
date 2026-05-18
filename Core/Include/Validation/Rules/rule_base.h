#ifndef RULE_BASE_H
#define RULE_BASE_H

#include <cstddef>

namespace validation {

  namespace rules {
    /**
     * @brief Base structure for all validation rules.
     * 
     * Provides the compile-time capacity mechanism that allows the framework
     * to statically allocate arrays for validation results, avoiding dynamic
     * memory allocations during rule execution.
     * 
     * @tparam MaxErrors The maximum number of validation errors this rule (and its children) can produce.
     */
    template<std::size_t MaxErrors>
    struct RuleBase {
      /// The maximum number of errors this rule can generate.
      static constexpr std::size_t capacity = MaxErrors;
    };
  }

}

#endif // RULE_BASE_H