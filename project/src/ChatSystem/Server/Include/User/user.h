#ifndef USER_H
#define USER_H

#include <Tags/tag.h>
#include <User/i_user_role.h>
#include <User/user_action.h>
#include <User/user_id.h>
#include <User/user_params.h>
#include <User/user_validator.h>
#include <Validation/validation.h>
#include <memory>
#include <stdexcept>
#include <string>

/**
 * @brief Domain entity representing a User within the system.
 * The User class encapsulates all user-related state and strictly enforces
 * its invariants through a generic validation policy provided at compile time.
 * State mutations are validated instantly via the `TUserParamsValidator`.
 * * @tparam TUserParamsValidator The validator policy type ensuring data integrity.
 */
template<UserValidatorFor<UserParams> TUserParamsValidator>
class User {
public:

  /**
   * @brief Default move constructor.
   * Efficiently transfers ownership of a user's data.
   */
  User(User&&) = default;

  /**
   * @brief Default move assignment operator.
   * Efficiently transfers ownership of a user's data to an existing instance.
   */
  User& operator=(User&&) = default;

  /**
   * @brief Deleted copy constructor.
   * Copying is strictly disabled because we never want to accidentally duplicate
   * one user's identity and data to another. Each User instance must be strictly unique.
   */
  User(const User&) = delete;

  /**
   * @brief Deleted copy assignment operator.
   * Copy assignment is strictly disabled to prevent overwriting a user's unique
   * identity and data with another user's information.
   */
  User& operator=(const User&) = delete;

  /**
   * @brief Factory method to safely construct a User object.
   * Fully validates the provided DTO before allowing construction.
   * * @param params The fully populated DTO representing the user state.
   * @param validator The validator instance to check the parameters against.
   * @return A valid, fully-constructed User instance.
   * @throws std::invalid_argument if the parameters fail validation.
   */
  [[nodiscard]] static User Create(UserParams params, const TUserParamsValidator& validator);

  /**
   * @brief Checks if the user's role permits a specific action.
   * @param action The action attempting to be performed.
   * @return true if permitted, false otherwise.
   */
  [[nodiscard]] bool CanPerform(UserAction action) const;

  /// @brief Retrieves the unique identifier of the user.
  [[nodiscard]] const UserId& GetId() const;

  /// @brief Retrieves the unique tag assigned to the user.
  [[nodiscard]] const Tag& GetTag() const;

  /// @brief Retrieves the login name of the user.
  [[nodiscard]] const std::string& GetLogin() const;

  /// @brief Retrieves the hashed password of the user.
  [[nodiscard]] const std::string& GetPasswordHash() const;

  /// @brief Retrieves the public key of the user.
  [[nodiscard]] const std::string& GetPublicKey() const;

  /// @brief Retrieves the assigned role of the user.
  [[nodiscard]] const IUserRole& GetRole() const;

  /**
   * @brief Mutates the user's role after strictly validating the new value.
   * @param role The new role to assign (takes ownership).
   * @param validator The validator to verify the new role.
   * @throws std::invalid_argument if the role fails validation.
   */
  void SetRole(std::unique_ptr<IUserRole> role, const TUserParamsValidator& validator);

  /**
   * @brief Mutates the user's login after strictly validating the new value.
   * @param login The new login string.
   * @param validator The validator to verify the new login.
   * @throws std::invalid_argument if the login fails validation.
   */
  void SetLogin(std::string login, const TUserParamsValidator& validator);

  /**
   * @brief Mutates the user's password hash after strictly validating the new value.
   * @param hash The new password hash string.
   * @param validator The validator to verify the new hash.
   * @throws std::invalid_argument if the hash fails validation.
   */
  void SetPasswordHash(std::string hash, const TUserParamsValidator& validator);

private:
  /**
   * @brief Private constructor called only after factory validation succeeds.
   * @param params Validated parameters.
   */
  explicit User(UserParams params)
    : id_{ std::move(params.id) }
    , tag_{ std::move(params.tag) }
    , login_{ std::move(params.login) }
    , password_hash_{ std::move(params.password_hash) }
    , public_key_{ std::move(params.public_key) }
    , role_{ std::move(params.role) } {
  }

  UserId id_;
  Tag tag_;
  std::string login_;
  std::string password_hash_;
  std::string public_key_;
  std::unique_ptr<IUserRole> role_;
};

template<UserValidatorFor<UserParams> TUserParamsValidator>
auto User<TUserParamsValidator>::Create(UserParams params, const TUserParamsValidator& validator) {
  if (const auto result = validator.Validate(params); !result.Ok()) {
    throw std::invalid_argument{ result.Summary() };
  }

  return User{ std::move(params) };
}

template <UserValidatorFor<UserParams> TUserParamsValidator>
bool User<TUserParamsValidator>::CanPerform(const UserAction action) const {
  return role_->CanPerform(action);
}
template <UserValidatorFor<UserParams> TUserParamsValidator>
const UserId& User<TUserParamsValidator>::GetId() const {
    return id_;
}
template <UserValidatorFor<UserParams> TUserParamsValidator>
const Tag& User<TUserParamsValidator>::GetTag() const {
    return tag_;
}
template <UserValidatorFor<UserParams> TUserParamsValidator>
const std::string& User<TUserParamsValidator>::GetLogin() const {
    return login_;
}
template <UserValidatorFor<UserParams> TUserParamsValidator>
const std::string& User<TUserParamsValidator>::GetPasswordHash() const {
    return password_hash_;
}
template <UserValidatorFor<UserParams> TUserParamsValidator>
const std::string& User<TUserParamsValidator>::GetPublicKey() const {
    return public_key_;
}
template <UserValidatorFor<UserParams> TUserParamsValidator>
const IUserRole& User<TUserParamsValidator>::GetRole() const {
    return *role_;
}

template <UserValidatorFor<UserParams> TUserParamsValidator>
void User<TUserParamsValidator>::SetRole(std::unique_ptr<IUserRole> role,
  const TUserParamsValidator& validator) {
  validation::SetOrThrow(*this, &User::role_, std::move(role), validator.GetRoleRule(),
    "Invalid Role");
}

template <UserValidatorFor<UserParams> TUserParamsValidator>
void User<TUserParamsValidator>::SetLogin(std::string login,
  const TUserParamsValidator& validator) {
  validation::SetOrThrow(*this, &User::login_, std::move(login), validator.GetLoginRule(),
    "Invalid Login");
}

template <UserValidatorFor<UserParams> TUserParamsValidator>
void User<TUserParamsValidator>::SetPasswordHash(std::string hash,
  const TUserParamsValidator& validator) {
  validation::SetOrThrow(*this, &User::password_hash_, std::move(hash),
    validator.GetPasswordHashRule(), "Invalid Password Hash");
}

#endif  // USER_H