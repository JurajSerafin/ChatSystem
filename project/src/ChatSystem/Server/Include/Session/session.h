#ifndef SESSION_H
#define SESSION_H

#include <Session/session_id.h>
#include <Session/session_params.h>
#include <Session/session_validator.h>
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
 *
 * @tparam TSessionParamsValidator The validator policy type ensuring data integrity.
 */
template<SessionValidatorFor<SessionParams> TSessionParamsValidator>
class Session {
public:

  /**
   * @brief Default move constructor.
   * Efficiently transfers ownership of a session's data.
   */
  Session(Session&&) = default;

  /**
   * @brief Default move assignment operator.
   * Efficiently transfers ownership of session data to an existing instance.
   */
  Session& operator=(Session&&) = default;

  /**
   * @brief Deleted copy constructor.
   * Copying is strictly disabled. A session token represents a unique, active
   * authentication grant. Copying it could lead to accidental session hijacking
   * or duplicate tracking within the system.
   */
  Session(const Session&) = delete;

  /**
   * @brief Deleted copy assignment operator.
   * Copy assignment is strictly disabled to prevent overwriting an existing
   * active session with another session's data.
   */
  Session& operator=(const Session&) = delete;

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
  [[nodiscard]] static Session Create(SessionParams params, const TSessionParamsValidator& validator);

  /// @brief Retrieves the unique identifier of the session.
  [[nodiscard]] const SessionId& GetId() const;

  /// @brief Retrieves the identifier of the user associated with the session.
  [[nodiscard]] const UserId& GetUserId() const;

  /// @brief Retrieves the session authentication token.
  [[nodiscard]] const std::string& GetToken() const;

  /// @brief Retrieves the expiration timestamp of the session.
  [[nodiscard]] std::chrono::system_clock::time_point ExpiresAt() const;

  /// @brief Retrieves the creation timestamp of the session.
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

template<SessionValidatorFor<SessionParams> TSessionParamsValidator>
Session<TSessionParamsValidator> Session<TSessionParamsValidator>::Create(SessionParams params, const TSessionParamsValidator& validator) {
  if (const auto result = validator.Validate(params); !result.Ok()) {
    throw std::invalid_argument{ result.Summary() };
  }

  return Session{ std::move(params) };
}

template<SessionValidatorFor<SessionParams> TSessionParamsValidator>
const SessionId& Session<TSessionParamsValidator>::GetId() const {
  return id_;
}

template<SessionValidatorFor<SessionParams> TSessionParamsValidator>
const UserId& Session<TSessionParamsValidator>::GetUserId() const {
  return user_id_;
}

template<SessionValidatorFor<SessionParams> TSessionParamsValidator>
const std::string& Session<TSessionParamsValidator>::GetToken() const {
  return token_;
}

template<SessionValidatorFor<SessionParams> TSessionParamsValidator>
std::chrono::system_clock::time_point Session<TSessionParamsValidator>::ExpiresAt() const {
  return expires_at_;
}

template<SessionValidatorFor<SessionParams> TSessionParamsValidator>
std::chrono::system_clock::time_point Session<TSessionParamsValidator>::CreatedAt() const {
  return created_at_;
}

template<SessionValidatorFor<SessionParams> TSessionParamsValidator>
bool Session<TSessionParamsValidator>::IsExpired() const {
  return std::chrono::system_clock::now() >= expires_at_;
}

template<SessionValidatorFor<SessionParams> TSessionParamsValidator>
void Session<TSessionParamsValidator>::Refresh(std::chrono::system_clock::duration duration) {
  expires_at_ = std::chrono::system_clock::now() + duration;
}

template<SessionValidatorFor<SessionParams> TSessionParamsValidator>
Session<TSessionParamsValidator>::Session(SessionParams params)
  : id_{ std::move(params.id) },
  user_id_{ std::move(params.user_id) },
  token_{ std::move(params.token) },
  expires_at_{ params.expires_at },
  created_at_{ params.created_at } {}

#endif  // SESSION_H