#ifndef SESSION_VALIDATOR_H
#define SESSION_VALIDATOR_H

#include <Session/session_params.h>
#include <Validation/validation.h>
#include <chrono>
#include <concepts>
#include <string>
#include <type_traits>


/**
 * @brief Validator for SessionParams objects.
 *
 * Ensures that session data satisfies all required constraints before
 * a Session entity is created.
 *
 * Validation includes:
 * - Valid session id
 * - Valid user id
 * - Token presence and minimum length
 * - Expiration timestamp correctness
 * - Timeline correctness (created_at <= expires_at)
 */
class SessionValidator : public IValidator<SessionParams, 5> {
public:
  /**
   * @brief Validates the entire SessionParams object.
   * @param params The session parameters to validate.
   * @return A ValidationResult containing any aggregated errors.
   */
  [[nodiscard]] constexpr validation::ValidationResult<kMaxErrors> Validate(const SessionParams& params) const override;

  /// Minimum allowed length for session tokens to enforce basic security.
  static constexpr size_t kMinTokenLength = 32;

  /// @brief Retrieves the raw validation rule for the authentication token.
  static constexpr auto GetTokenRule();

  /// @brief Retrieves the raw validation rule for the creation timestamp.
  static constexpr auto GetCreatedAtRule(std::chrono::system_clock::time_point other);

  /// @brief Retrieves the raw validation rule for the expiration timestamp.
  static constexpr auto GetExpiresAtRule();
};


constexpr auto SessionValidator::GetTokenRule() {
  return validation::rules::NotEmpty &&
    validation::rules::MinLength<kMinTokenLength>;
}

constexpr auto SessionValidator::GetExpiresAtRule() {
  return validation::rules::TimePointMustBeSet;
}

constexpr auto SessionValidator::GetCreatedAtRule(std::chrono::system_clock::time_point other) {
  return validation::rules::TimePointMustBeSet &&
    validation::rules::IsLessThanInclusive(other);
}

constexpr validation::ValidationResult<SessionValidator::kMaxErrors> SessionValidator::Validate(const SessionParams& params) const {
  const auto rules =
    (VALIDATION_BIND_FIELD(SessionParams, token) | GetTokenRule()) &&
    (VALIDATION_BIND_FIELD(SessionParams, expires_at) | GetExpiresAtRule()) &&
    (VALIDATION_BIND_FIELD(SessionParams, created_at) | GetCreatedAtRule(params.expires_at));

  return rules(params);
}

#endif  // SESSION_VALIDATOR_H