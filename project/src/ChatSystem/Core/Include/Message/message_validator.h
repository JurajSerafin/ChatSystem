#ifndef MESSAGE_VALIDATOR_H
#define MESSAGE_VALIDATOR_H

#include <Message/message_params.h>
#include <Validation/validation.h>

/**
 * @brief Validator for MessageParams objects.
 *
 * This class implements validation logic for user-related data,
 * ensuring that all required fields meet defined constraints.
 *
 * The validation checks include:
 * - Point of creation set, other than the epoch
 */
class MessageValidator : public IValidator<MessageParams, 1> {
public:
  /**
   * @brief Validates the entire MessageParams object.
   * @param params The user parameters to validate.
   * @return A ValidationResult containing any aggregated errors.
   */
  [[nodiscard]] constexpr validation::ValidationResult<kMaxErrors> Validate(const MessageParams& params) const override;

  /// @brief Retrieves the raw validation rule for the message creation time point.
  static constexpr auto GetCreatedAtRule();
};
constexpr auto MessageValidator::GetCreatedAtRule() {
  return  validation::rules::TimePointMustBeSet;
}

constexpr validation::ValidationResult<MessageValidator::kMaxErrors> MessageValidator::Validate(const MessageParams& params) const {
  constexpr auto rules =
    (VALIDATION_BIND_FIELD(MessageParams, created_at) | GetCreatedAtRule());

  return rules(params);
}


#endif  // MESSAGE_VALIDATOR_H
