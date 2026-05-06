#ifndef PQXX_USER_REPOSITORY_H
#define PQXX_USER_REPOSITORY_H

#include "Repositories/i_user_repository.h"

class Transaction;

class QueryParams;

class IConnectionPool;

class PqxxUserRepository : public IUserRepository {
public:
  std::optional<User> FindById(const UserId& id) override;

  std::optional<User> FindByLogin(const std::string& login) override;

  std::optional<User> FindByTag(const std::string& tag) override;

  std::vector<User> Search(const std::string& query) override;

  User Create(const User& user) override;

  void Update(const User& user) override;

  void DeleteById(const UserId& id) override;

  void StorePublicKey(const UserId& id, const std::string& publicKey) override;

  std::optional<std::string> GetPublicKey(const UserId& id) override;

  explicit PqxxUserRepository(IConnectionPool* connectionPoolObs);

private:
  IConnectionPool* connection_pool_obs_;

  std::optional<User> TryFetchUser(std::string_view query, const QueryParams& params, Transaction&& tx);

  std::vector<User> TryFetchUsers(std::string_view query, Transaction&& tx);
};

 

#endif // PQXX_USER_REPOSITORY_H