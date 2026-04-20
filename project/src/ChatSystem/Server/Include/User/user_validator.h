#ifndef USER_VALIDATOR_H
#define USER_VALIDATOR_H

#include <User/user_params.h>
#include <Validation/validation.h>
#include <concepts>
#include <string>
#include <type_traits>

/**
 * @brief Concept defining the requirements for a User validator.
 * Extends the basic `ValidatorFor` concept to ensure the validator also provides
 * access to individual field rules. This allows domain objects (like `User`)
 * to validate individual fields during state mutations (e.g., `SetLogin`) without
 * requiring a full `UserParams` DTO.
 *
 * @tparam TValidator The validator type being checked.
 * @tparam TParams The parameter DTO type (expected to structurally match UserParams).
 */
template<typename TValidator, typename TParams>
concept UserValidatorFor = validation::ValidatorFor<TValidator, TParams>&& requires(TValidator validator, TParams params) {
  // Ensure the params object contains the expected fields
  { params.id } -> std::convertible_to<UserId>;
  { params.tag } -> std::convertible_to<Tag>;
  { params.login } -> std::convertible_to<std::string>;
  { params.password_hash } -> std::convertible_to<std::string>;
  { params.public_key } -> std::convertible_to<std::string>;
  { params.role } -> std::convertible_to<std::unique_ptr<IUserRole>>;

  // Ensure the validator exposes raw rules for each field that evaluate to a boolean Ok()
  { validator.GetIdRule()(params.id).Ok() } -> std::convertible_to<bool>;
  { validator.GetTagRule()(params.tag).Ok() } -> std::convertible_to<bool>;
  { validator.GetLoginRule()(params.login).Ok() } -> std::convertible_to<bool>;
  { validator.GetPasswordHashRule()(params.password_hash).Ok() } -> std::convertible_to<bool>;
  { validator.GetPublicKeyRule()(params.public_key).Ok() } -> std::convertible_to<bool>;
  { validator.GetRoleRule()(params.role).Ok() } -> std::convertible_to<bool>;
};

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
class UserValidator : public IValidator<UserParams, 8> {
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

  /// @brief Retrieves the raw validation rule for the user ID.
  static constexpr auto GetIdRule();

  /// @brief Retrieves the raw validation rule for the user tag.
  static constexpr auto GetTagRule();

  /// @brief Retrieves the raw validation rule for the user login.
  static constexpr auto GetLoginRule();

  /// @brief Retrieves the raw validation rule for the user password hash.
  static constexpr auto GetPasswordHashRule();

  /// @brief Retrieves the raw validation rule for the user public key.
  static constexpr auto GetPublicKeyRule();

  /// @brief Retrieves the raw validation rule for the user role.
  static constexpr auto GetRoleRule();
};

#endif  // USER_VALIDATOR_H