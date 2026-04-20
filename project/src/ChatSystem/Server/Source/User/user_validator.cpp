#include <User/user_validator.h>

#include <Validation/validation.h>

using namespace validation::rules;

validation::ValidationResult<UserValidator::kMaxErrors> UserValidator::Validate(const UserParams& params) const {
  auto rules =
    (BIND_FIELD(UserParams, id) | GetIdRule()) &&
    (BIND_FIELD(UserParams, tag) | GetTagRule()) &&
    (BIND_FIELD(UserParams, login) | GetLoginRule()) &&
    (BIND_FIELD(UserParams, password_hash) | GetPasswordHashRule()) &&
    (BIND_FIELD(UserParams, public_key) | GetPublicKeyRule()) &&
    (BIND_FIELD(UserParams, role) | GetRoleRule());

  return rules(params);
}

constexpr auto UserValidator::GetIdRule() {
  return IsValid;
}

constexpr auto UserValidator::GetTagRule() {
  return IsValid;
}

constexpr auto UserValidator::GetLoginRule() {
  return NotEmpty && MinLength<kMinLoginLength> && MaxLength<kMaxLoginLength>;
}

constexpr auto UserValidator::GetPasswordHashRule() {
  return NotEmpty;
}

constexpr auto UserValidator::GetPublicKeyRule() {
  return NotEmpty;
}

constexpr auto UserValidator::GetRoleRule() {
  return IsNotNull;
}