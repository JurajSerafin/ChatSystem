#ifndef CHAT_VALIDATOR_FOR_H
#define CHAT_VALIDATOR_FOR_H

#include "Types/chat_type_variant.h"
#include "Validation/validation.h"

#include <chrono>
#include <concepts>
#include <type_traits>

class ChatId;
/**
 * @brief Concept defining the requirements for a Chat validator.
 * Extends the basic `ValidatorFor` concept to ensure the validator provides
 * access to individual chat field rules. This guarantees that
 * domain objects can validate individual fields during state mutations.
 *
 * @tparam TValidator The validator type being checked.
 * @tparam TParams The parameter DTO type.
 */
template<typename TValidator, typename TParams>
concept ChatValidatorFor = validation::ValidatorFor<TValidator, TParams>&& requires(TValidator validator, TParams params) {
  // Ensure the params object contains the expected fields
  { params.id } -> std::convertible_to<ChatId>;
  { params.created_at } -> std::convertible_to<std::chrono::system_clock::time_point>;
  { params.type } -> std::convertible_to<ChatTypeVariant>;


  // Ensure the validator exposes raw rules for each field that evaluate to a boolean Ok()
  { validator.GetCreatedAtRule()(params.created_at).Ok() } -> std::same_as<bool>;
};

#endif // CHAT_VALIDATOR_FOR_H