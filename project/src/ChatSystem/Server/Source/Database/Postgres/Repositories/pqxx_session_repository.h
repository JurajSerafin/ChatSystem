#ifndef PQXX_SESSION_REPOSITORY_H
#define PQXX_SESSION_REPOSITORY_H

#include <Database/i_connection_pool.h>
#include <Repositories/i_session_repository.h>

class Transaction;
class QueryParams;

/**
 * @brief Concrete PostgreSQL implementation of the server-side session repository.
 * * Manages fast lookups for incoming API request authentication and
 * handles bulk-deletions of expired or revoked tokens.
 */
class PqxxSessionRepository : public ISessionRepository {
public:
  /**
   * @brief Constructs the repository with a thread-safe connection pool.
   * @param connectionPoolObs Observer pointer to the active PostgreSQL connection pool.
   */
  explicit PqxxSessionRepository(IConnectionPool* connectionPoolObs);

  void Add(const Session& session) override;
  std::optional<Session> FindByToken(const std::string& token) override;
  std::vector<Session> FindByUserId(const UserId& userId) override;
  void DeleteByToken(const std::string& token) override;
  void DeleteAllForUser(const UserId& userId) override;
  void DeleteExpired() override;

private:
  IConnectionPool* connection_pool_obs_;

  /**
   * @brief Internal helper to execute a parameterized query and safely extract a single Session.
   * @param query The SQL query string.
   * @param params The bound SQL parameters.
   * @param tx The active database transaction wrapper (consumed via rvalue).
   * @return An optional Session domain entity if exactly one row matched.
   */
  std::optional<Session> TryFetchSession(std::string_view query, const QueryParams& params, Transaction&& tx);

  /**
   * @brief Internal helper to execute a parameterized query and extract multiple Sessions.
   * @param query The SQL query string.
   * @param params The bound SQL parameters.
   * @param tx The active database transaction wrapper.
   * @return A populated vector of Session domain entities.
   */
  std::vector<Session> TryFetchSessions(std::string_view query, const QueryParams& params, Transaction&& tx);
};

#endif // PQXX_SESSION_REPOSITORY_H