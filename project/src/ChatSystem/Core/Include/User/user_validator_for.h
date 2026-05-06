#ifndef USER_VALIDATOR_FOR_H
#define USER_VALIDATOR_FOR_H

#include "Validation/Concepts/validator_concepts.h"
#include "Roles/user_role_variant.h"

#include <chrono>
#include <concepts>
#include <string>
#include <type_traits>

namespace tags {
class UserTag;
} // namespace tags

class UserId;


/**
 * @brief Concept defining the requirements for a User validator.
 * Extends the basic `ValidatorFor` concept to ensure the validator also provides
 * access to individual field rules. This allows domain objects to validate
 * individual fields during state mutations (e.g., `SetLogin`) without
 * requiring a full `UserParams` DTO.
 *
 * @tparam TValidator The validator type being checked.
 * @tparam TParams The parameter DTO type (expected to structurally match UserParams).
 */
template<typename TValidator, typename TParams>
concept UserValidatorFor = validation::ValidatorFor<TValidator, TParams>&& requires(TValidator validator, TParams params) {
  // Ensure the params object contains the expected fields
  { params.id } -> std::convertible_to<UserId>;
  { params.tag } -> std::convertible_to<tags::UserTag>;
  { params.login } -> std::convertible_to<std::string>;
  { params.password_hash } -> std::convertible_to<std::string>;
  { params.public_key } -> std::convertible_to<std::string>;
  { params.role } -> std::convertible_to<UserRoleVariant>;
  { params.created_at } -> std::convertible_to<std::chrono::system_clock::time_point>;

  // Ensure the validator exposes raw rules for each field that evaluate to a boolean Ok()
  { validator.GetLoginRule()(params.login).Ok() } -> std::same_as<bool>;
  { validator.GetPasswordHashRule()(params.password_hash).Ok() } -> std::same_as<bool>;
  { validator.GetPublicKeyRule()(params.public_key).Ok() } -> std::same_as<bool>;
  { validator.GetCreatedAtRule()(params.created_at).Ok() }-> std::same_as<bool>;
};

#endif // USER_VALIDATOR_FOR_H