#ifndef CHAT_VALIDATOR_H
#define CHAT_VALIDATOR_H

#include "chat_params.h"

#include "Validation/validation.h"

/**
 * @brief Validator for Chat objects.
 *
 * Ensures that chat data satisfies all required constraints before
 * a Chat entity is created.
 *
 * Validation includes:
 * - Creation timestamp set
 */
class ChatValidator : public IValidator<ChatParams, 1> {
public:
  /**
   * @brief Validates the entire ChatParams object.
   * @param params The chat parameters to validate.
   * @return A ValidationResult containing any aggregated errors.
   */
  [[nodiscard]] constexpr validation::ValidationResult<kMaxErrors> Validate(const ChatParams& params) const override;

  /// @brief Retrieves the raw validation rule for the creation timestamp.
  static constexpr auto GetCreatedAtRule();

};

constexpr auto ChatValidator::GetCreatedAtRule() {
  return validation::rules::TimePointMustBeSet;
}

constexpr validation::ValidationResult<ChatValidator::kMaxErrors> ChatValidator::Validate(const ChatParams& params) const {
  constexpr auto rules = (VALIDATION_BIND_FIELD(ChatParams, created_at) | GetCreatedAtRule());

  return rules(params);
}

#endif  // CHAT_VALIDATOR_H