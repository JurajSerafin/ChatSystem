#include <Session/session_validator.h>
#include <Validation/validation.h>

using namespace validation::rules;

validation::ValidationResult<SessionValidator::kMaxErrors> SessionValidator::Validate(const SessionParams& params) const {

  auto rules =
    (BIND_FIELD(SessionParams, id) | GetIdRule()) &&
    (BIND_FIELD(SessionParams, user_id) | GetUserIdRule()) &&
    (BIND_FIELD(SessionParams, token) | GetTokenRule()) &&
    (BIND_FIELD(SessionParams, expires_at) | GetExpiresAtRule()) &&
    (BIND_FIELD(SessionParams, created_at) | GetCreatedAtRule(params.expires_at));

  return rules(params);
}

constexpr auto SessionValidator::GetIdRule() {
  return IsValid;
}

constexpr auto SessionValidator::GetUserIdRule() {
  return IsValid;
}

constexpr auto SessionValidator::GetTokenRule() {
  return NotEmpty && MinLength<kMinTokenLength>;
}

constexpr auto SessionValidator::GetExpiresAtRule() {
  return TimePointMustBeSet;
}

constexpr auto SessionValidator::GetCreatedAtRule(std::chrono::system_clock::time_point other) {
  return TimePointMustBeSet && IsLessThanInclusive(other);
}