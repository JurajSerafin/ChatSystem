#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include <Repositories/i_session_repository.h>
#include <Repositories/i_user_repository.h>
#include <Services/i_auth_service.h>
#include <Services/i_encryption_service.h>
#include <Session/session.h>
#include <Session/session_id.h>
#include <Session/session_params.h>
#include <Tags/user_tag.h>
#include <User/user_id.h>
#include <User/user_params.h>
#include <Validation/validation.h>
#include <chrono>
#include <string>
#include <optional>

/**
 * @brief Concrete implementation of IAuthService.
 *
 * Handles the full authentication lifecycle: registration, login,
 * logout, token validation, and password changes.
 *
 * @tparam TUserValidator Type of the validator used for User entities.
 * @tparam TSessionValidator Type of the validator used for Session entities.
 *
 * @see IAuthService
 * @see IUserRepository
 * @see ISessionRepository
 * @see IEncryptionService
 */
template<UserValidatorFor<UserParams> TUserValidator, SessionValidatorFor<SessionParams> TSessionValidator>
class AuthService : public IAuthService {
public:
  /**
   * @brief Constructs the AuthService with all required dependencies.
   *
   * @param userRepo Repository for user persistence and lookup.
   * @param sessionRepo Repository for session persistence and lookup.
   * @param encryptionService Service for hashing, key generation, and verification.
   * @param userValidator Validator applied during user registration and mutation.
   * @param sessionValidator Validator applied during session creation.
   */
  AuthService(
    IUserRepository& userRepo,
    ISessionRepository& sessionRepo,
    IEncryptionService& encryptionService,
    const TUserValidator& userValidator,
    const TSessionValidator& sessionValidator
  );

  AuthService(const AuthService&) = delete;

  AuthService& operator=(const AuthService&) = delete;

  AuthService(AuthService&&) = delete;

  AuthService& operator=(AuthService&&) = delete;

  /**
   * @brief Registers a new user with the given login and password.
   *
   * Checks for login uniqueness, generates a unique tag, hashes the
   * password, generates a key pair, validates, and persists the user.
   *
   * @param login The desired login name.
   * @param password The plaintext password to hash and store.
   * @return The newly created and persisted User.
   * @throws std::invalid_argument if the login is already taken or validation fails.
   */
  User RegisterUser(const std::string& login, const std::string& password) override;

  /**
   * @brief Authenticates a user and creates a new session.
   *
   * @param login The user's login name.
   * @param password The plaintext password to verify.
   * @return A newly created Session containing the access token.
   * @throws std::invalid_argument if credentials are invalid.
   */
  Session Login(const std::string& login, const std::string& password) override;

  /**
   * @brief Invalidates the session associated with the given token.
   * @param token The session token to invalidate.
   */
  void Logout(const std::string& token) override;

  /**
   * @brief Validates a session token and returns the associated user.
   *
   * @param token The session token to validate.
   * @return The User associated with the token, or std::nullopt if the token is invalid or expired.
   */
  std::optional<User> ValidateToken(const std::string& token) override;

  /**
   * @brief Changes the password for the given user.
   *
   * @param userId The ID of the user changing their password.
   * @param oldPassword The current plaintext password for verification.
   * @param newPassword The new plaintext password to hash and store.
   * @throws std::invalid_argument if the user is not found or the current password is incorrect.
   */
  void ChangePassword(UserId userId, const std::string& oldPassword, const std::string& newPassword) override;

private:
  /**
   * @brief Generates a tag unique among all existing users.
   *
   * Retries up to kMaxTagRetries times on collision.
   *
   * @param login The login used as the tag base.
   * @return A unique tags::UserTag for the new user.
   * @throws std::runtime_error if a unique tag cannot be generated within the retry limit.
   */
  tags::UserTag GenerateUniqueTag(const std::string& login) const;

  /**
   * @brief Generates a cryptographically random session token.
   * @return A UUID string representing a session token.
   */
  static std::string GenerateToken();

  /// Duration after which a session expires.
  static constexpr auto kSessionDuration = std::chrono::hours{ 24 };

  /**
   * Reference fields are intentional here because:
   * - All dependencies are required and never null
   * - All dependencies outlive this service — they are constructed before
   * it and destroyed after it in server_main.cpp → no dangling risk.
   * - This class is non-copyable and non-movable by design
   */
  IUserRepository& user_repo_;
  ISessionRepository& session_repo_;
  IEncryptionService& encryption_service_;

  const TUserValidator& user_validator_;
  const TSessionValidator& session_validator_;
};

template<UserValidatorFor<UserParams> TUserValidator, SessionValidatorFor<SessionParams> TSessionValidator>
AuthService<TUserValidator, TSessionValidator>::AuthService(
  IUserRepository& userRepo,
  ISessionRepository& sessionRepo,
  IEncryptionService& encryptionService,
  const TUserValidator& userValidator,
  const TSessionValidator& sessionValidator)
  : user_repo_{ userRepo }
  , session_repo_{ sessionRepo }
  , encryption_service_{ encryptionService }
  , user_validator_{ userValidator }
  , session_validator_{ sessionValidator } {
}


template<UserValidatorFor<UserParams> TUserValidator, SessionValidatorFor<SessionParams> TSessionValidator>
void AuthService<TUserValidator, TSessionValidator>::Logout(const std::string& token) {
  session_repo_.DeleteByToken(token);
}

template<UserValidatorFor<UserParams> TUserValidator, SessionValidatorFor<SessionParams> TSessionValidator>
std::string AuthService<TUserValidator, TSessionValidator>::GenerateToken() {
  return SessionId::Generate().ToString();
}

template<UserValidatorFor<UserParams> TUserValidator, SessionValidatorFor<SessionParams> TSessionValidator>
User AuthService<TUserValidator, TSessionValidator>::RegisterUser(const std::string& login, const std::string& password) {
  if (user_repo_.FindByLogin(login).has_value()) {
    throw std::invalid_argument{ "Login already taken" };
  }

  tags::UserTag tag = GenerateUniqueTag(login);
  std::string password_hash = encryption_service_.HashPassword(password);
  KeyPair key_pair = encryption_service_.GenerateKeyPair();

  User user = User::Create({
    .id = UserId::Generate(),
    .tag = std::move(tag),
    .login = login,
    .password_hash = std::move(password_hash),
    .public_key = key_pair.GetPublicKey(),
    .role = std::make_unique<RegularUserRole>(),
    .created_at = std::chrono::system_clock::now()
    }, user_validator_);

  return user_repo_.Create(std::move(user));
}

template<UserValidatorFor<UserParams> TUserValidator, SessionValidatorFor<SessionParams> TSessionValidator>
Session AuthService<TUserValidator, TSessionValidator>::Login(const std::string& login, const std::string& password) {
  const auto user = user_repo_.FindByLogin(login);

  if (!user.has_value()) {
    throw std::invalid_argument{ "Invalid credentials" };
  }

  if (!encryption_service_.VerifyPassword(password, user->GetPasswordHash()))
    throw std::invalid_argument{ "Invalid credentials" };

  const auto now = std::chrono::system_clock::now();

  Session session = Session::Create({
    .id = SessionId::Generate(),
    .user_id = user->GetId(),
    .token = GenerateToken(),
    .expires_at = now + kSessionDuration,
    .created_at = now }, session_validator_);

  return session_repo_.Create(std::move(session));
}

template<UserValidatorFor<UserParams> TUserValidator, SessionValidatorFor<SessionParams> TSessionValidator>
std::optional<User> AuthService<TUserValidator, TSessionValidator>::ValidateToken(const std::string& token) {
  const auto session = session_repo_.FindByToken(token);

  if (!session.has_value()) {
    return std::nullopt;
  }

  if (session->IsExpired()) {
    return std::nullopt;
  }

  return user_repo_.FindById(session->GetUserId());
}

template<UserValidatorFor<UserParams> TUserValidator, SessionValidatorFor<SessionParams> TSessionValidator>
void AuthService<TUserValidator, TSessionValidator>::ChangePassword(const UserId userId, const std::string& oldPassword, const std::string& newPassword) {
  auto user = user_repo_.FindById(userId);

  if (!user.has_value()) {
    throw std::invalid_argument{ "User not found" };
  }

  if (!encryption_service_.VerifyPassword(oldPassword, user->GetPasswordHash()))
    throw std::invalid_argument{ "Invalid current password" };

  user->SetPasswordHash(encryption_service_.HashPassword(newPassword), user_validator_);

  user_repo_.Update(*user);
}

template<UserValidatorFor<UserParams> TUserValidator, SessionValidatorFor<SessionParams> TSessionValidator>
tags::UserTag AuthService<TUserValidator, TSessionValidator>::GenerateUniqueTag(const std::string& login) const {
  constexpr int kMaxRetries = 100;

  for (int i = 0; i < kMaxRetries; ++i) {
    if (tags::UserTag tag = tags::GenerateFromLogin(login); !user_repo_.FindByTag(tag.ToString()).has_value()) {
      return tag;
    }
  }

  throw std::runtime_error{ "Failed to generate unique tag after " + std::to_string(kMaxRetries) + " attempts" };
}

#endif  // AUTH_SERVICE_H