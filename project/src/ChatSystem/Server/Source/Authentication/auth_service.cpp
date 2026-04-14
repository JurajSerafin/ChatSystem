#include "User/regular_user_role.h"

#include <Authentication/auth_service.h>

#include <memory>
#include <Authentication/tags.h>

User AuthService::RegisterUser(const std::string& login, const std::string& password) {
  if (user_repo_.FindByLogin(login).has_value()) {
    throw std::invalid_argument{ "Login already taken" };
  }

  Tag tag = GenerateUniqueTag(login);

  std::string password_hash = encryption_service_.HashPassword(password);

  KeyPair key_pair = encryption_service_.GenerateKeyPair();

  User user = User::Create({
    .id = UserId::Generate(),
    .tag = std::move(tag),
    .login = login,
    .password_hash = std::move(password_hash),
    .public_key = key_pair.GetPublicKey(),
    .role = std::make_unique<RegularUserRole>() 
    }, user_validator_);

  return user_repo_.Create(std::move(user));
}

Session AuthService::Login(const std::string& login, const std::string& password) {
  const auto user = user_repo_.FindByLogin(login);

  if (!user.has_value()) {
    throw std::invalid_argument{ "Invalid credentials" };
  }

  if (!encryption_service_.VerifyPassword(password, user->GetPasswordHash()))
      throw std::invalid_argument{"Invalid credentials"};

  const auto now = std::chrono::system_clock::now();

  Session session = Session::Create({
    .id = SessionId::Generate(),
    .user_id = user->GetId(),
    .token = GenerateToken(),
    .expires_at = now + kSessionDuration,
    .created_at = now}, session_validator_);

  return session_repo_.Create(std::move(session));
}

std::optional<User> AuthService::ValidateToken(const std::string& token) {
  const auto session = session_repo_.FindByToken(token);

  if (!session.has_value()) {
      return std::nullopt;
  }

  if (session->IsExpired()) {
      return std::nullopt;
  }

  return user_repo_.FindById(session->GetUserId());
}

void AuthService::ChangePassword(const UserId userId, const std::string& oldPassword, const std::string& newPassword) {
  auto user = user_repo_.FindById(userId);

  if (!user.has_value()) {
    throw std::invalid_argument{ "User not found" };
  }

  if (!encryption_service_.VerifyPassword(oldPassword, user->GetPasswordHash()))
      throw std::invalid_argument{"Invalid current password"};

  user->SetPasswordHash(encryption_service_.HashPassword(newPassword));

  user_repo_.Update(*user);
}

Tag AuthService::GenerateUniqueTag(const std::string& login) const {
    constexpr int kMaxRetries = 10;

    for (int i = 0; i < kMaxRetries; ++i) {
      if (Tag tag = tags::GenerateFromLogin(login); !user_repo_.FindByTag(tag.value).has_value()) {
        return tag;
      }
    }

    throw std::runtime_error{"Failed to generate unique tag after " + std::to_string(kMaxRetries) + " attempts"};
}