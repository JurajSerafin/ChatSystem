#ifndef PQXX_USER_REPOSITORY_H
#define PQXX_USER_REPOSITORY_H

#include "Repositories/i_user_repository.h"

class Transaction;
class QueryParams;
class IConnectionPool;

/**
 * @brief Concrete PostgreSQL implementation of the server-side user repository.
 * * Handles persisting user credentials, public keys, and performing paginated
 * ILIKE text searches across the user directory.
 */
class PqxxUserRepository : public IUserRepository {
public:
  /**
   * @brief Constructs the repository with a thread-safe connection pool.
   * @param connectionPoolObs Observer pointer to the active PostgreSQL connection pool.
   */
  explicit PqxxUserRepository(IConnectionPool* connectionPoolObs);

  std::optional<User> FindById(const UserId& id) override;
  std::optional<User> FindByLogin(const std::string& login) override;
  std::optional<User> FindByTag(const tags::UserTag& tag) override;
  std::vector<User> Search(const std::string& query, std::size_t limit, std::size_t offset) override;
  void Add(const User& user) override;
  void Update(const User& user) override;
  void DeleteById(const UserId& id) override;

private:
  IConnectionPool* connection_pool_obs_;

  /**
   * @brief Internal helper to execute a parameterized query and safely extract a single User profile.
   * @param query The SQL query string.
   * @param params The bound SQL parameters.
   * @param tx The active database transaction wrapper (consumed via rvalue).
   * @return An optional User domain entity if exactly one row matched.
   */
  std::optional<User> TryFetchUser(std::string_view query, const QueryParams& params, Transaction&& tx);

  /**
   * @brief Internal helper to execute a parameterized query and extract multiple User profiles.
   * @param query The SQL query string.
   * @param params The bound SQL parameters.
   * @param tx The active database transaction wrapper (consumed via rvalue).
   * @return A populated vector of User domain entities.
   */
  std::vector<User> TryFetchUsers(std::string_view query, const QueryParams& params, Transaction&& tx);
};

#endif // PQXX_USER_REPOSITORY_H