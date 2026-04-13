#ifndef USER_VALIDATOR_H
#define USER_VALIDATOR_H

#include <User/user_params.h>
#include <Validation/i_validator.h>
#include <Validation/validation_result.h>

/**
 * @brief Validator for UserParams objects.
 *
 * This class implements validation logic for user-related data,
 * ensuring that all required fields meet defined constraints.
 *
 * The validation checks include:
 * - Valid identifier
 * - Non-empty tag
 * - Login length constraints
 * - Presence of password hash
 * - Presence of public key
 * - Valid role assignment
 */
class UserValidator : public IValidator<UserParams> {
public:
  /**
   * @brief Validates user parameters.
   *
   * Executes all validation rules on the provided UserParams instance
   * and aggregates any errors into a ValidationResult.
   *
   * @param params User parameters to validate.
   * @return ValidationResult containing all detected validation errors.
   */
  [[nodiscard]] ValidationResult Validate(const UserParams& params) const override;

private:
  /// Minimum allowed length for the login field.
  static constexpr size_t kMinLoginLength = 3;

  /// Maximum allowed length for the login field.
  static constexpr size_t kMaxLoginLength = 32;

  /**
   * @brief Validates the user identifier.
   *
   * Ensures that the ID is valid (non-empty and properly initialized).
   *
   * @param params User parameters being validated.
   * @param validationResult Accumulates validation errors.
   */
  static void ValidateId(const UserParams& params, ValidationResult& validationResult);

  /**
   * @brief Validates the user tag.
   *
   * Ensures that the tag is not empty.
   *
   * @param params User parameters being validated.
   * @param validationResult Accumulates validation errors.
   */
  static void ValidateTag(const UserParams& params, ValidationResult& validationResult);

  /**
   * @brief Validates the login field.
   *
   * Ensures that:
   * - Login is not empty
   * - Login length is within allowed bounds
   *
   * @param params User parameters being validated.
   * @param validationResult Accumulates validation errors.
   */
  static void ValidateLogin(const UserParams& params, ValidationResult& validationResult);

  /**
   * @brief Validates the password hash.
   *
   * Ensures that the password hash is present.
   *
   * @param params User parameters being validated.
   * @param validationResult Accumulates validation errors.
   */
  static void ValidatePasswordHash(const UserParams& params, ValidationResult& validationResult);

  /**
   * @brief Validates the public key.
   *
   * Ensures that the public key is present.
   *
   * @param params User parameters being validated.
   * @param validationResult Accumulates validation errors.
   */
  static void ValidatePublicKey(const UserParams& params, ValidationResult& validationResult);

  /**
   * @brief Validates the user role.
   *
   * Ensures that the role is set (not null).
   *
   * @param params User parameters being validated.
   * @param validationResult Accumulates validation errors.
   */
  static void ValidateRole(const UserParams& params, ValidationResult& validationResult);
};


inline ValidationResult UserValidator::Validate(const UserParams& params) const {
  ValidationResult result;

  ValidateId(params, result);
  ValidateTag(params, result);
  ValidateLogin(params, result);
  ValidatePasswordHash(params, result);
  ValidatePublicKey(params, result);
  ValidateRole(params, result);

  return result;
}

inline void UserValidator::ValidateId(const UserParams& params, ValidationResult& validationResult) {
  if (!params.id.IsValid()) {
    validationResult.AddError("id", "cannot be empty");
  }
}

inline void UserValidator::ValidateTag(const UserParams& params, ValidationResult& validationResult) {
  if (params.tag.value.empty()) {
    validationResult.AddError("tag", "cannot be empty");
  }
}

inline void UserValidator::ValidateLogin(
  const UserParams& params, ValidationResult& validationResult) {
  if (params.login.empty()) {
    validationResult.AddError("login", "cannot be empty");
    return;
  }

  if (params.login.size() < kMinLoginLength) {
    validationResult.AddError("login", "must be at least 3 characters");
  }

  if (params.login.size() > kMaxLoginLength) {
    validationResult.AddError("login", "must be at most 32 characters");
  }
}

inline void UserValidator::ValidatePasswordHash(const UserParams& params, ValidationResult& validationResult) {
  if (params.password_hash.empty()) {
    validationResult.AddError("password_hash", "cannot be empty");
  }
}

inline void UserValidator::ValidatePublicKey(const UserParams& params, ValidationResult& validationResult) {
  if (params.public_key.empty()) {
    validationResult.AddError("public_key", "cannot be empty");
  }
}

inline void UserValidator::ValidateRole(const UserParams& params, ValidationResult& validationResult) {
  if (!params.role) {
    validationResult.AddError("role", "cannot be null");
  }
}

#endif  // USER_VALIDATOR_H