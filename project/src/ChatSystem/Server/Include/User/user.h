#ifndef USER_H
#define USER_H

#include <Tags/user_tag.h>
#include <User/i_user_role.h>
#include <User/user_action.h>
#include <User/user_id.h>
#include <User/user_params.h>
#include <User/user_validator.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

/**
 * @brief Domain entity representing a User within the system.
 * The User class encapsulates all user-related state and strictly enforces
 * its invariants on creation through a generic validation policy provided
 * at compile time.
 * 
 * To enforce unique instances, this class is made move-only.

 */
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
  template<UserValidatorFor<UserParams> TUserParamsValidator>
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
  [[nodiscard]] const tags::UserTag& GetTag() const;

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
  void SetRole(std::unique_ptr<IUserRole> role);

  /**
   * @brief Mutates the user's login after strictly validating the new value.
   * @param login The new login string.
   * @param validator The validator to verify the new login.
   * @throws std::invalid_argument if the login fails validation.
   */
  void SetLogin(std::string login);

  /**
   * @brief Mutates the user's password hash after strictly validating the new value.
   * @param hash The new password hash string.
   * @param validator The validator to verify the new hash.
   * @throws std::invalid_argument if the hash fails validation.
   */
  void SetPasswordHash(std::string hash);

private:
  /**
   * @brief Private constructor called only after factory validation succeeds.
   * @param params Validated parameters.
   */
  explicit User(UserParams params);

  UserId id_;
  tags::UserTag tag_;
  std::string login_;
  std::string password_hash_;
  std::string public_key_;
  std::unique_ptr<IUserRole> role_;
};

template<UserValidatorFor<UserParams> TUserParamsValidator>
User User::Create(UserParams params, const TUserParamsValidator& validator) {
  if (const auto result = validator.Validate(params); !result.Ok()) {
    throw std::invalid_argument{ result.Summary() };
  }

  return User{ std::move(params) };
}

inline bool User::CanPerform(const UserAction action) const {
  return role_->CanPerform(action);
}

inline const UserId& User::GetId() const {
  return id_;
}

inline const tags::UserTag& User::GetTag() const {
  return tag_;
}

inline const std::string& User::GetLogin() const {
  return login_;
}

inline const std::string& User::GetPasswordHash() const {
  return password_hash_;
}

inline const std::string& User::GetPublicKey() const {
  return public_key_;
}

inline const IUserRole& User::GetRole() const {
  return *role_;
}

inline void User::SetRole(std::unique_ptr<IUserRole> role) {
  role_ = std::move(role);
}

inline void User::SetLogin(std::string login) {
  login_ = std::move(login);
}

inline void User::SetPasswordHash(std::string hash) {
  password_hash_ = std::move(hash);
}

inline User::User(UserParams params)
  : id_{std::move(params.id)},
    tag_{std::move(params.tag)},
    login_{std::move(params.login)},
    password_hash_{std::move(params.password_hash)},
    public_key_{std::move(params.public_key)},
    role_{std::move(params.role)} {}

#endif  // USER_H