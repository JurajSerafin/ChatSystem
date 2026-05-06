#ifndef I_SESSION_REPOSITORY_H
#define I_SESSION_REPOSITORY_H

#include <optional>
#include <vector>
#include <string>


#include "Repositories/i_sesion_repository.h"

class Session;

class UserId;

class Transaction;

class QueryParams;

class IConnectionPool;


class PqxxSessionRepository : public ISessionRepository {
public:

  Session Create(const Session& session) override;

  std::optional<Session> FindByToken(const std::string& token) override;

  std::vector<Session> FindByUserId(UserId userId) override;

  void DeleteByToken(const std::string& token) override;

  void DeleteAllForUser(UserId userId) override;

  explicit PqxxSessionRepository(IConnectionPool* connection_pool_obs);

private:
  IConnectionPool* connection_pool_obs_;

  std::optional<Session> TryFetchSession(std::string_view query, const QueryParams& params, Transaction&& tx);
  std::vector<Session> TryFetchSessions(std::string_view query, const QueryParams& params, Transaction&& tx);
};

#endif // I_SESSION_REPOSITORY_H

