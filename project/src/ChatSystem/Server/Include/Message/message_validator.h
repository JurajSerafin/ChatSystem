#ifndef MESSAGE_VALIDATOR_H
#define MESSAGE_VALIDATOR_H

#include "message_id.h"
#include "message_type.h"

#include <Chat/chat_id.h>
#include <Message/message_params.h>
#include <User/user_id.h>
#include <Validation/validation.h>
#include <chrono>
#include <concepts>
#include <string>
#include <type_traits>
#include <vector>


/**
 * @brief Concept defining the requirements for a Message validator.
 * Extends the basic `ValidatorFor` concept to ensure the validator also provides
 * access to individual field rules. This allows domain objects to validate
 * individual fields during state mutations (e.g., `SetId`) without
 * requiring a full `MessageParams` DTO.
 *
 * @tparam TValidator The validator type being checked.
 * @tparam TParams The parameter DTO type (expected to structurally match MessageParams).
 */
template<typename TValidator, typename TParams>
concept MessageValidatorFor = validation::ValidatorFor<TValidator, TParams>&& requires(TValidator validator, TParams params) {
  // Ensure the params object contains the expected fields
  { params.id } -> std::convertible_to<MessageId>;
  { params.chat_id } -> std::convertible_to<ChatId>;
  { params.sender_id } -> std::convertible_to<UserId>;
  { params.content } -> std::convertible_to<std::string>;
  { params.type } -> std::convertible_to<MessageType>;
  { params.created_at } -> std::convertible_to< std::chrono::system_clock::time_point>;

  // Ensure the validator exposes raw rules for each field that evaluate to a boolean Ok()
  { validator.GetIdRule()(params.id).Ok() } -> std::convertible_to<bool>;
  { validator.GetChatIdRule()(params.chat_id).Ok() } -> std::convertible_to<bool>;
  { validator.GetSenderIdRule()(params.sender_id).Ok() } -> std::convertible_to<bool>;
  { validator.GetContentRule()(params.content).Ok() } -> std::convertible_to<bool>;
  { validator.GetTypeRule()(params.type).Ok() } -> std::convertible_to<bool>;
  { validator.GetCreatedAtRule()(params.created_at).Ok() } -> std::convertible_to<bool>;
};

/**
 * @brief Validator for MessageParams objects.
 *
 * This class implements validation logic for user-related data,
 * ensuring that all required fields meet defined constraints.
 *
 * The validation checks include:
 * - Valid id
 * - Valid chat id
 * - Valid sender id
 * - Non-empty content, with maximal size kMaxContentLength
 * - Non-empty tag
 * - Point of creation set, other than the epoch
 */
class MessageValidator : public IValidator<MessageParams, 6> {
private:
  /// Maximum allowed length of message content.
  static constexpr size_t kMaxContentLength = 10000;

public:
  /**
   * @brief Validates the entire MessageParams object.
   * @param params The user parameters to validate.
   * @return A ValidationResult containing any aggregated errors.
   */
  [[nodiscard]] constexpr validation::ValidationResult<kMaxErrors> Validate(const MessageParams& params) const override;

  /// @brief Retrieves the raw validation rule for the message ID.
  static constexpr auto GetIdRule();

  /// @brief Retrieves the raw validation rule for the chat id.
  static constexpr auto GetChatIdRule();

  /// @brief Retrieves the raw validation rule for the sender id.
  static constexpr auto GetSenderIdRule();

  /// @brief Retrieves the raw validation rule for the message content.
  static constexpr auto GetContentRule();

  /// @brief Retrieves the raw validation rule for the message type.
  static constexpr auto GetTypeRule();

  /// @brief Retrieves the raw validation rule for the message creation time point.
  static constexpr auto GetCreatedAtRule();
};

constexpr auto MessageValidator::GetIdRule() {
  return validation::rules::IsValid;
}

constexpr auto MessageValidator::GetChatIdRule() {
  return  validation::rules::IsValid;
}

constexpr auto MessageValidator::GetSenderIdRule() {
  return  validation::rules::IsValid;
}

constexpr auto MessageValidator::GetContentRule() {
  return  validation::rules::NotEmpty && 
    validation::rules::MaxLength<kMaxContentLength>;
}

constexpr auto MessageValidator::GetTypeRule() {
  return  validation::rules::TrueRule;
}

constexpr auto MessageValidator::GetCreatedAtRule() {
  return  validation::rules::TimePointMustBeSet;
}

constexpr validation::ValidationResult<MessageValidator::kMaxErrors> MessageValidator::Validate(const MessageParams& params) const {
  auto rules =
    (VALIDATION_BIND_FIELD(MessageParams, id) | GetIdRule()) &&
    (VALIDATION_BIND_FIELD(MessageParams, chat_id) | GetChatIdRule()) &&
    (VALIDATION_BIND_FIELD(MessageParams, sender_id) | GetSenderIdRule()) &&
    (VALIDATION_BIND_FIELD(MessageParams, content) | GetContentRule()) &&
    (VALIDATION_BIND_FIELD(MessageParams, type) | GetTypeRule()) &&
    (VALIDATION_BIND_FIELD(MessageParams, created_at) | GetCreatedAtRule());

  return rules(params);
}


#endif  // MESSAGE_VALIDATOR_H