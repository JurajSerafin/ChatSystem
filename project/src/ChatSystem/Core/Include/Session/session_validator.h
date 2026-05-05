#ifndef SESSION_VALIDATOR_H
#define SESSION_VALIDATOR_H

#include <Session/session_params.h>
#include <Validation/validation.h>
#include <chrono>
#include <concepts>
#include <string>
#include <type_traits>

/**
 * @brief Concept defining the requirements for a Session validator.
 * Extends the basic `ValidatorFor` concept to ensure the validator provides
 * access to individual session field rules. This guarantees that
 * domain objects can validate individual fields during state mutations.
 * 
 * @tparam TValidator The validator type being checked.
 * @tparam TParams The parameter DTO type.
 */
template<typename TValidator, typename TParams>
concept SessionValidatorFor = validation::ValidatorFor<TValidator, TParams>&& requires(TValidator validator, TParams params) {
  // Ensure the params object contains the expected fields
  { params.id } -> std::convertible_to<SessionId>;
  { params.user_id } -> std::convertible_to<UserId>;
  { params.token } -> std::convertible_to<std::string>;
  { params.expires_at } -> std::convertible_to<std::chrono::system_clock::time_point>;
  { params.created_at } -> std::convertible_to<std::chrono::system_clock::time_point>;

  // Ensure the validator exposes raw rules for each field that evaluate to a boolean Ok()
  { validator.GetIdRule()(params.id).Ok() } -> std::convertible_to<bool>;
  { validator.GetUserIdRule()(params.user_id).Ok() } -> std::convertible_to<bool>;
  { validator.GetTokenRule()(params.token).Ok() } -> std::convertible_to<bool>;
  { validator.GetExpiresAtRule()(params.expires_at).Ok() } -> std::convertible_to<bool>;

  // GetCreatedAtRule requires a threshold time point to construct the rule
  { validator.GetCreatedAtRule(params.expires_at)(params.created_at).Ok() } -> std::convertible_to<bool>;
};

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
class SessionValidator : public IValidator<SessionParams, 7> {
public:
  /**
   * @brief Validates the entire SessionParams object.
   * @param params The session parameters to validate.
   * @return A ValidationResult containing any aggregated errors.
   */
  [[nodiscard]] constexpr validation::ValidationResult<kMaxErrors> Validate(const SessionParams& params) const override;

  /// Minimum allowed length for session tokens to enforce basic security.
  static constexpr size_t kMinTokenLength = 32;

  /// @brief Retrieves the raw validation rule for the session ID.
  static constexpr auto GetIdRule();

  /// @brief Retrieves the raw validation rule for the user ID.
  static constexpr auto GetUserIdRule();

  /// @brief Retrieves the raw validation rule for the authentication token.
  static constexpr auto GetTokenRule();

  /// @brief Retrieves the raw validation rule for the creation timestamp.
  static constexpr auto GetCreatedAtRule(std::chrono::system_clock::time_point other);

  /// @brief Retrieves the raw validation rule for the expiration timestamp.
  static constexpr auto GetExpiresAtRule();
};

constexpr auto SessionValidator::GetIdRule() {
  return validation::rules::IsValid;
}

constexpr auto SessionValidator::GetUserIdRule() {
  return validation::rules::IsValid;
}

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

  auto rules =
    (VALIDATION_BIND_FIELD(SessionParams, id) | GetIdRule()) &&
    (VALIDATION_BIND_FIELD(SessionParams, user_id) | GetUserIdRule()) &&
    (VALIDATION_BIND_FIELD(SessionParams, token) | GetTokenRule()) &&
    (VALIDATION_BIND_FIELD(SessionParams, expires_at) | GetExpiresAtRule()) &&
    (VALIDATION_BIND_FIELD(SessionParams, created_at) | GetCreatedAtRule(params.expires_at));

  return rules(params);
}

#endif  // SESSION_VALIDATOR_H