#ifndef CHAT_VALIDATOR_H
#define CHAT_VALIDATOR_H

#include "chat_params.h"

#include "Validation/validation.h"
#include <chrono>

#include <concepts>
#include <string>
#include <type_traits>

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
  { params.name } -> std::convertible_to<std::optional<std::string>>;


  // Ensure the validator exposes raw rules for each field that evaluate to a boolean Ok()
  { validator.GetIdRule()(params.id).Ok() } -> std::convertible_to<bool>;
  { validator.GetCreatedAtRule()(params.created_at).Ok() } -> std::convertible_to<bool>;
  { validator.GetNameRule()(params.name).Ok() } -> std::convertible_to<bool>;
};

/**
 * @brief Validator for Chat objects.
 *
 * Ensures that chat data satisfies all required constraints before
 * a Chat entity is created.
 *
 * Validation includes:
 * - Valid chat id
 * - Creation timestamp set
 * - Optionally, name set
 */
class ChatValidator : public IValidator<ChatParams, 2> {
public:
  /**
   * @brief Validates the entire ChatParams object.
   * @param params The chat parameters to validate.
   * @return A ValidationResult containing any aggregated errors.
   */
  [[nodiscard]] constexpr validation::ValidationResult<kMaxErrors> Validate(const ChatParams& params) const override;


  /// @brief Retrieves the raw validation rule for the chat ID.
  static constexpr auto GetIdRule();

  /// @brief Retrieves the raw validation rule for the creation timestamp.
  static constexpr auto GetCreatedAtRule();

  /// @brief Retrieves the raw validation rule for the chat name.
  static constexpr auto GetNameRule();
};

constexpr auto ChatValidator::GetIdRule() {
  return validation::rules::IsValid;
}

constexpr auto ChatValidator::GetCreatedAtRule() {
  return validation::rules::TimePointMustBeSet;
}

constexpr auto ChatValidator::GetNameRule() {
  return validation::rules::TrueRule;
}

constexpr validation::ValidationResult<ChatValidator::kMaxErrors> ChatValidator::Validate(const ChatParams& params) const {

  auto rules =
    (VALIDATION_BIND_FIELD(ChatParams, id) | GetIdRule()) &&
    (VALIDATION_BIND_FIELD(ChatParams, created_at) | GetCreatedAtRule()) &&
    (VALIDATION_BIND_FIELD(ChatParams, name) | GetNameRule());

  return rules(params);
}

#endif  // CHAT_VALIDATOR_H