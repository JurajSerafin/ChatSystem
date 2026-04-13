#ifndef MESSAGE_VALIDATOR_H
#define MESSAGE_VALIDATOR_H

#include <chrono>

#include <Message/message_params.h>
#include <Validation/i_validator.h>
#include <Validation/validation_result.h>

/**
 * @brief Validator for MessageParams objects.
 *
 * Ensures that message data meets required constraints before processing.
 * Validation includes identifiers, content rules, and timestamp presence.
 */
class MessageValidator : public IValidator<MessageParams> {
public:
  /**
   * @brief Validates message parameters.
   *
   * Runs all validation checks on the provided MessageParams instance
   * and returns a ValidationResult containing any errors found.
   *
   * @param params Message parameters to validate.
   * @return ValidationResult containing validation errors.
   */
  [[nodiscard]] ValidationResult Validate(const MessageParams& params) const override;

private:
  /**
   * @brief Validates the message identifier.
   */
  static void ValidateId(const MessageParams& params, ValidationResult& validationResult);

  /**
   * @brief Validates the chat identifier.
   */
  static void ValidateChatId(const MessageParams& params, ValidationResult& validationResult);

  /**
   * @brief Validates the sender identifier.
   */
  static void ValidateSenderId(const MessageParams& params, ValidationResult& validationResult);

  /**
   * @brief Validates the message content.
   *
   * Ensures content is not empty and does not exceed maximum length.
   */
  static void ValidateContent(const MessageParams& params, ValidationResult& validationResult);

  /**
   * @brief Validates the creation timestamp.
   *
   * Ensures the timestamp is set (not default-initialized).
   */
  static void ValidateCreatedAt(const MessageParams& params, ValidationResult& validationResult);

  /// Maximum allowed length of message content.
  static constexpr size_t kMaxContentLength = 10000;
};


inline ValidationResult MessageValidator::Validate(const MessageParams& params) const {
  ValidationResult result;

  ValidateId(params, result);
  ValidateChatId(params, result);
  ValidateSenderId(params, result);
  ValidateContent(params, result);
  ValidateCreatedAt(params, result);

  return result;
}

inline void MessageValidator::ValidateId(const MessageParams& params, ValidationResult& validationResult) {
  if (!params.id.IsValid()) {
    validationResult.AddError("id", "cannot be empty");
  }
}

inline void MessageValidator::ValidateChatId(const MessageParams& params, ValidationResult& validationResult) {
  if (!params.chat_id.IsValid()) {
    validationResult.AddError("chat_id", "cannot be empty");
  }
}

inline void MessageValidator::ValidateSenderId(const MessageParams& params, ValidationResult& validationResult) {
  if (!params.sender_id.IsValid()) {
    validationResult.AddError("sender_id", "cannot be empty");
  }
}

inline void MessageValidator::ValidateContent(const MessageParams& params, ValidationResult& validationResult) {
  if (params.content.empty()) {
    validationResult.AddError("content", "cannot be empty");
    return;
  }

  if (params.content.size() > kMaxContentLength) {
    validationResult.AddError("content", "exceeds maximum length of 10000 characters");
  }
}

inline void MessageValidator::ValidateCreatedAt(const MessageParams& params, ValidationResult& validationResult) {
  if (params.created_at == std::chrono::system_clock::time_point{}) {
    validationResult.AddError("created_at", "must be set");
  }
}

#endif  // MESSAGE_VALIDATOR_H