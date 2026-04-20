#ifndef VALIDATION_H
#define VALIDATION_H

#include <Validation/Concepts/rule_concepts.h>
#include <Validation/Concepts/validator_concepts.h>

#include <Validation/Core/validation_error.h>
#include <Validation/Core/validation_result.h>

#include <Validation/Rules/and_rule.h>
#include <Validation/Rules/bound_rule.h>
#include <Validation/Rules/field.h>
#include <Validation/Rules/is_not_null.h>
#include <Validation/Rules/is_valid.h>
#include <Validation/Rules/max_length.h>
#include <Validation/Rules/min_length.h>
#include <Validation/Rules/not_empty.h>
#include <Validation/Rules/rule_base.h>
#include <Validation/Rules/time_point_must_be_set.h>
#include <Validation/Rules/is_less_than_inclusive.h>

#include <Validation/utils.h>
#include <Validation/i_validator.h>

/**
 * @brief Macro to quickly bind an object member to a Field validation rule.
 * Automatically stringifies the member name for use in validation error messages,
 * reducing the type and namespace names boilerplate.
 * 
 * @param Object The class or struct type.
 * @param Member The specific member to bind.
 */
#define BIND_FIELD(Object, Member) validation::rules::Field{&Object::Member, #Member}

#endif // VALIDATION_H