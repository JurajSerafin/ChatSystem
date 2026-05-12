#ifndef PQXX_SESSION_REPOSITORY_H
#define PQXX_SESSION_REPOSITORY_H

#include <Database/i_connection_pool.h>
#include <Repositories/i_session_repository.h>

class Transaction;
class QueryParams;

class PqxxSessionRepository : public ISessionRepository {
public:
  explicit PqxxSessionRepository(IConnectionPool* connectionPoolObs);

  Session Create(const Session& session) override;

  std::optional<Session> FindByToken(const std::string& token) override;

  std::vector<Session> FindByUserId(const UserId& userId) override;

  void DeleteByToken(const std::string& token) override;
  void DeleteAllForUser(const UserId& userId) override;
  void DeleteExpired() override;

private:
  IConnectionPool* connection_pool_obs_;

  std::optional<Session> TryFetchSession(std::string_view query, const QueryParams& params, Transaction&& tx);

  std::vector<Session> TryFetchSessions(std::string_view query, const QueryParams& params, Transaction&& tx);
};

#endif // PQXX_SESSION_REPOSITORY_H