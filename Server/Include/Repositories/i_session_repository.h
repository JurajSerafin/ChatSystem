#ifndef I_SESSION_REPOSITORY_H
#define I_SESSION_REPOSITORY_H

#include <optional>
#include <vector>
#include <string>

#include <Session/session.h>
#include <User/user_id.h>

/**
 * @brief Server-side repository interface for managing user authentication sessions.
 * * Handles the persistence, lookup, and invalidation of secure session tokens
 * used to authenticate incoming API requests.
 */
class ISessionRepository {
public:
  virtual ~ISessionRepository() = default;

  /**
   * @brief Persists a newly created session (e.g., following a successful login).
   * @param session The session entity containing the token, user ID, and expiration date.
   */
  virtual void Add(const Session& session) = 0;

  /**
   * @brief Looks up an active session by its secure token string.
   * * Primarily used by the server's authentication layer to validate requests.
   * @param token The raw session token string.
   * @return An optional containing the Session if found and valid.
   */
  virtual std::optional<Session> FindByToken(const std::string& token) = 0;

  /**
   * @brief Retrieves all currently active sessions for a specific user.
   * * Useful for multi-device management or displaying "Active Devices" to the user.
   * @param userId The unique identifier of the user.
   * @return A vector of active sessions.
   */
  virtual std::vector<Session> FindByUserId(const UserId& userId) = 0;

  /**
   * @brief Invalidates and removes a specific session from the database (e.g., standard logout).
   * @param token The session token to destroy.
   */
  virtual void DeleteByToken(const std::string& token) = 0;

  /**
   * @brief Invalidates all active sessions globally for a specific user.
   * @param userId The unique identifier of the user.
   */
  virtual void DeleteAllForUser(const UserId& userId) = 0;

  /**
   * @brief Sweeps the database and removes any sessions that have passed their expiration date.
   */
  virtual void DeleteExpired() = 0;
};

#endif // I_SESSION_REPOSITORY_H