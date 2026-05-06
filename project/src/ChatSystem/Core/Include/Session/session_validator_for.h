#ifndef SESSION_VALIDATOR_FOR_H
#define SESSION_VALIDATOR_FOR_H

#include <Validation/validation.h>
#include <chrono>
#include <concepts>
#include <string>
#include <type_traits>

class UserId;
class SessionId;

/**
 * @brief Concept defining the requirements for a Session validator.
 * Extends the basic `ValidatorFor` concept to ensure the validator provides
 * access to individual session field rules. This guarantees that
 * domain objects can validate individual fields during state mutations.
 *
 * @tparam TValidator The validator type being checked.
 * @tparam TParams The parameter DTO type.
 */
template<typename TValidator, typename TParams>
concept SessionValidatorFor = validation::ValidatorFor<TValidator, TParams>&& requires(TValidator validator, TParams params) {
  // Ensure the params object contains the expected fields
  { params.id } -> std::convertible_to<SessionId>;
  { params.user_id } -> std::convertible_to<UserId>;
  { params.token } -> std::convertible_to<std::string>;
  { params.expires_at } -> std::convertible_to<std::chrono::system_clock::time_point>;
  { params.created_at } -> std::convertible_to<std::chrono::system_clock::time_point>;

  // Ensure the validator exposes raw rules for each field that evaluate to a boolean Ok()
  { validator.GetTokenRule()(params.token).Ok() } -> std::same_as<bool>;
  { validator.GetExpiresAtRule()(params.expires_at).Ok() } -> std::same_as<bool>;

  // GetCreatedAtRule requires a threshold time point to construct the rule
  { validator.GetCreatedAtRule(params.expires_at)(params.created_at).Ok() } -> std::same_as<bool>;
};

#endif // SESSION_VALIDATOR_FOR_H