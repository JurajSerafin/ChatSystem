#ifndef SESSION_VALIDATOR_H
#define SESSION_VALIDATOR_H

#include <Session/session_params.h>
#include <Validation/i_validator.h>
#include <Validation/validation_result.h>
#include <chrono>

/**
 * @brief Validator for SessionParams objects.
 *
 * Ensures that session data satisfies all required constraints before
 * a Session entity is created.
 *
 * Validation includes:
 * - Valid session identifier
 * - Valid user identifier
 * - Token presence and minimum length
 * - Expiration timestamp correctness
 */
class SessionValidator : public IValidator<SessionParams> {
public:
  /**
   * @brief Validates session parameters.
   *
   * Executes all validation rules on the provided SessionParams instance
   * and aggregates any errors into a ValidationResult.
   *
   * @param params Session parameters to validate.
   * @return ValidationResult containing all detected validation errors.
   */
  [[nodiscard]] ValidationResult Validate(const SessionParams& params) const override {

    ValidationResult result;

    ValidateId(params, result);
    ValidateUserId(params, result);
    ValidateToken(params, result);
    ValidateExpiry(params, result);

    return result;
  }

private:
  /**
   * @brief Validates the session identifier.
   *
   * Ensures that the ID is valid.
   */
  static void ValidateId(const SessionParams& params, ValidationResult& validationResult) {
    if (!params.id.IsValid()) {
      validationResult.AddError("id", "cannot be empty");
    }
  }

  /**
   * @brief Validates the user identifier.
   *
   * Ensures that the associated user ID is valid.
   */
  static void ValidateUserId(const SessionParams& params, ValidationResult& validationResult) {
    if (!params.user_id.IsValid()) {
      validationResult.AddError("user_id", "cannot be empty");
    }
  }

  /**
   * @brief Validates the session token.
   *
   * Ensures that:
   * - Token is not empty
   * - Token meets minimum length requirements
   */
  static void ValidateToken(const SessionParams& params, ValidationResult& validationResult) {
    if (params.token.empty()) {
      validationResult.AddError("token", "cannot be empty");
    }
    else if (params.token.size() < kMinTokenLength) {
      validationResult.AddError("token", "token too short");
    }
  }

  /**
   * @brief Validates session expiration.
   *
   * Ensures that:
   * - Expiration timestamp is set
   * - Expiration occurs after creation time
   */
  static void ValidateExpiry(const SessionParams& params, ValidationResult& validationResult) {
    if (params.created_at == std::chrono::system_clock::time_point{}) {
      validationResult.AddError("created_at", "must be set");
    }

    if (params.expires_at == std::chrono::system_clock::time_point{}) {
      validationResult.AddError("expires_at", "must be set");
    }

    if (params.expires_at <= params.created_at) {
      validationResult.AddError("expires_at", "must be after created_at");
    }
  }

  /// Minimum allowed length for the session token.
  static constexpr size_t kMinTokenLength = 32;
};

#endif  // SESSION_VALIDATOR_H