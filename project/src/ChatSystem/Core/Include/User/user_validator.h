#ifndef USER_VALIDATOR_H
#define USER_VALIDATOR_H

#include <User/user_params.h>
#include <Validation/validation.h>

/**
 * @brief Validator for UserParams objects.
 *
 * This class implements validation logic for user-related data,
 * ensuring that all required fields meet defined constraints.
 *
 * The validation checks include:
 * - Valid id
 * - Non-empty tag
 * - Login length constraints
 * - Presence of password hash
 * - Presence of public key
 * - Valid role assignment
 */
class UserValidator : public IValidator<UserParams, 6> {
private:
  /// Minimum allowed length for the login field.
  static constexpr std::size_t kMinLoginLength = 3;

  /// Maximum allowed length for the login field.
  static constexpr std::size_t kMaxLoginLength = 32;

public:
  /**
   * @brief Validates the entire UserParams object.
   * @param params The user parameters to validate.
   * @return A ValidationResult containing any aggregated errors.
   */
  [[nodiscard]] constexpr validation::ValidationResult<kMaxErrors> Validate(const UserParams& params) const override;

  /// @brief Retrieves the raw validation rule for the user login.
  static constexpr auto GetLoginRule();

  /// @brief Retrieves the raw validation rule for the user password hash.
  static constexpr auto GetPasswordHashRule();

  /// @brief Retrieves the raw validation rule for the user public key.
  static constexpr auto GetPublicKeyRule();

  static constexpr auto GetCreatedAtRule();
};

constexpr auto UserValidator::GetLoginRule() {
  return validation::rules::NotEmpty
    && validation::rules::MinLength<kMinLoginLength>
    && validation::rules::MaxLength<kMaxLoginLength>;
}

constexpr auto UserValidator::GetPasswordHashRule() {
  return validation::rules::NotEmpty;
}

constexpr auto UserValidator::GetPublicKeyRule() {
  return validation::rules::NotEmpty;
}

constexpr auto UserValidator::GetCreatedAtRule() {
  return validation::rules::TimePointMustBeSet;
}

constexpr validation::ValidationResult<UserValidator::kMaxErrors> UserValidator::Validate(const UserParams& params) const {
  constexpr auto rules =
    (VALIDATION_BIND_FIELD(UserParams, login) | GetLoginRule()) &&
    (VALIDATION_BIND_FIELD(UserParams, password_hash) | GetPasswordHashRule()) &&
    (VALIDATION_BIND_FIELD(UserParams, public_key) | GetPublicKeyRule()) &&
    (VALIDATION_BIND_FIELD(UserParams, created_at) | GetCreatedAtRule());

  return rules(params);
}



#endif  // USER_VALIDATOR_H