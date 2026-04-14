#ifndef SESSION_H
#define SESSION_H

#include <Session/session_id.h>
#include <Session/session_params.h>
#include <User/user_id.h>
#include <Validation/i_validator.h>
#include <chrono>
#include <stdexcept>
#include <string>

/**
 * @brief Represents an authenticated user session.
 *
 * A Session encapsulates authentication state for a user, including
 * a unique session token, expiration time, and creation timestamp.
 *
 * Sessions are created via the static Create() method, which enforces
 * validation through a provided validator.
 *
 * Instances are move-only and support expiration checks and refresh logic.
 */
class Session {
public:
  /**
   * @brief Creates a validated Session instance.
   *
   * Validates the provided parameters and constructs a Session if valid.
   *
   * @param params Session parameters used for construction.
   * @param validator Validator used to verify correctness.
   * @return Constructed Session object.
   * @throws std::invalid_argument if validation fails.
   */
  [[nodiscard]] static Session Create(SessionParams params, const IValidator<SessionParams>& validator);

  Session(Session&&) = default;
  Session& operator=(Session&&) = default;

  Session(const Session&) = delete;
  Session& operator=(const Session&) = delete;

  /// @return Unique identifier of the session.
  [[nodiscard]] const SessionId& GetId() const;

  /// @return Identifier of the user associated with the session.
  [[nodiscard]] const UserId& GetUserId() const;

  /// @return Session authentication token.
  [[nodiscard]] const std::string& GetToken() const;

  /// @return Expiration timestamp of the session.
  [[nodiscard]] std::chrono::system_clock::time_point ExpiresAt() const;

  /// @return Creation timestamp of the session.
  [[nodiscard]] std::chrono::system_clock::time_point CreatedAt() const;

  /**
   * @brief Checks whether the session is expired.
   *
   * @return true if current time is past expiration, false otherwise.
   */
  [[nodiscard]] bool IsExpired() const;

  /**
   * @brief Extends the session expiration time.
   *
   * Typically called on user activity to keep the session alive.
   *
   * @param duration Duration to extend from current time.
   */
  void Refresh(std::chrono::system_clock::duration duration);

private:
  /**
   * @brief Constructs a Session from parameters.
   *
   * Use Create() for validated construction.
   *
   * @param params Session parameters.
   */
  explicit Session(SessionParams params);

  SessionId id_;
  UserId user_id_;
  std::string token_;
  std::chrono::system_clock::time_point expires_at_;
  std::chrono::system_clock::time_point created_at_;
};

inline Session Session::Create(
  SessionParams params,
  const IValidator<SessionParams>& validator) {

  if (const auto result = validator.Validate(params); !result.IsValid()) {
    throw std::invalid_argument{ result.Summary() };
  }

  return Session{ std::move(params) };
}

inline const SessionId& Session::GetId() const {
  return id_;
}

inline const UserId& Session::GetUserId() const {
  return user_id_;
}

inline const std::string& Session::GetToken() const {
  return token_;
}

inline std::chrono::system_clock::time_point Session::ExpiresAt() const {
  return expires_at_;
}

inline std::chrono::system_clock::time_point Session::CreatedAt() const {
  return created_at_;
}

inline bool Session::IsExpired() const {
  return std::chrono::system_clock::now() >= expires_at_;
}

inline void Session::Refresh(std::chrono::system_clock::duration duration) {
  expires_at_ = std::chrono::system_clock::now() + duration;
}

inline Session::Session(SessionParams params)
  : id_{ std::move(params.id) },
  user_id_{ std::move(params.user_id) },
  token_{ std::move(params.token) },
  expires_at_{ params.expires_at },
  created_at_{ params.created_at } {
}

#endif  // SESSION_H