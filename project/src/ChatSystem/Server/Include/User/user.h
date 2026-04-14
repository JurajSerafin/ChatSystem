#ifndef USER_H
#define USER_H

#include <Tags/tag.h>
#include <User/i_user_role.h>
#include <User/user_action.h>
#include <User/user_id.h>
#include <User/user_params.h>
#include <Validation/i_validator.h>
#include <memory>
#include <stdexcept>
#include <string>

class User {
public:
  [[nodiscard]] static User Create(UserParams params, const IValidator<UserParams>& validator);

  User(User&&) = default;
  User& operator=(User&&) = default;
  User(const User&) = delete;
  User& operator=(const User&) = delete;

  [[nodiscard]] const UserId& GetId() const { return id_; }
  [[nodiscard]] const Tag& GetTag() const { return tag_; }
  [[nodiscard]] const std::string& GetLogin() const { return login_; }
  [[nodiscard]] const std::string& GetPasswordHash() const { return password_hash_; }
  [[nodiscard]] const std::string& GetPublicKey() const { return public_key_; }
  [[nodiscard]] const IUserRole& GetRole() const { return *role_; }

  void SetRole(std::unique_ptr<IUserRole> role) { role_ = std::move(role); }
  void SetLogin(std::string login,const IValidator<UserParams>& validator);

  void SetPasswordHash(std::string hash) {
    if (hash.empty())
      throw std::invalid_argument{ "User: password_hash cannot be empty" };
    password_hash_ = std::move(hash);
  }

  [[nodiscard]] bool CanPerform(UserAction action) const {
    return role_->CanPerform(action);
  }

private:
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

inline User User::Create(UserParams params, const IValidator<UserParams>& validator) {
  const auto result = validator.Validate(params);

  if (!result.IsValid()) {
    throw std::invalid_argument{ result.Summary() };
  }
  return User{ std::move(params) };
}

#endif  // USER_H