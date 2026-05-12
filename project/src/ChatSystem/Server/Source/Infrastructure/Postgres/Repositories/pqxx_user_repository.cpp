#include "Infrastructure/Postgres/Repositories/pqxx_user_repository.h"

#include "User/user.h"

#include <Database/query_params.h>
#include <Database/transaction.h>
#include <Infrastructure/Postgres/Mappers/user_mapper.h>
#include <stdexcept>

PqxxUserRepository::PqxxUserRepository(IConnectionPool* connectionPoolObs) : connection_pool_obs_{ connectionPoolObs } {
  if (!connection_pool_obs_) {
    throw std::invalid_argument("PqxxUserRepository requires a valid IConnectionPool pointer.");
  }
}

std::optional<User> PqxxUserRepository::TryFetchUser(std::string_view query, const QueryParams& params, Transaction&& tx) {
  const auto query_result = tx.Execute(query, params);

  if (query_result->IsEmpty()) {
    return std::nullopt;
  }

  std::optional<User> fetched_user = std::nullopt;

  query_result->First([&fetched_user](const IRow& row) {
    fetched_user = UserMapper{}.Map(row);
  });

  return fetched_user;
}

std::vector<User> PqxxUserRepository::TryFetchUsers(std::string_view query, const QueryParams& params, Transaction&& tx) {
  const auto query_result = tx.Execute(query, params);

  std::vector<User> fetched_users;

  if (query_result->IsEmpty()) {
    return fetched_users;
  }

  fetched_users.reserve(query_result->GetSize());

  UserMapper mapper{};

  query_result->ForEach([&fetched_users, &mapper](const IRow& row) {
    fetched_users.emplace_back(mapper.Map(row));
  });


  return fetched_users;
}

std::optional<User> PqxxUserRepository::FindById(const UserId& id) {
  return TryFetchUser(
    "SELECT * FROM users WHERE id = $1",
    QueryParams{}.BindParam(id.ToString()),
    Transaction{ std::move(connection_pool_obs_->Acquire()) }
  );
}

std::optional<User> PqxxUserRepository::FindByLogin(const std::string& login) {
  return TryFetchUser(
    "SELECT * FROM users WHERE login = $1",
    QueryParams{}.BindParam(login),
    Transaction{ std::move(connection_pool_obs_->Acquire()) }
  );
}

std::optional<User> PqxxUserRepository::FindByTag(const tags::UserTag& tag) {
  return TryFetchUser(
    "SELECT * FROM users WHERE tag = $1",
    QueryParams{}.BindParam(tag.ToString()),
    Transaction{ std::move(connection_pool_obs_->Acquire()) }
  );
}

std::vector<User> PqxxUserRepository::Search(const std::string& query, std::size_t limit, std::size_t offset) {
  const std::string sql = R"(
    SELECT * FROM users 
    WHERE login ILIKE $1 OR tag ILIKE $1 
    LIMIT $2 OFFSET $3
  )";

  std::string search_term = "%" + query + "%";

  const auto params = QueryParams{}
    .BindParam(std::move(search_term))
    .BindParam(limit)
    .BindParam(offset);

  return TryFetchUsers(sql, params, Transaction{ std::move(connection_pool_obs_->Acquire()) });
}

User PqxxUserRepository::Create(const User& user) {
  auto tx = Transaction{ std::move(connection_pool_obs_->Acquire()) };

  const std::string sql = R"(
    INSERT INTO users (id, login, tag, password_hash, public_key, role, created_at) 
    VALUES ($1, $2, $3, $4, $5, $6, $7) 
    RETURNING *
  )";

  const auto params = QueryParams{}
    .BindParam(user.GetId().ToString())
    .BindParam(user.GetLogin())
    .BindParam(user.GetTag().ToString())
    .BindParam(user.GetPasswordHash())
    .BindParam(user.GetPublicKey())
    .BindParam(std::string{ user.GetRoleStr() })
    .BindParam(user.CreatedAt());

  const auto query_result = tx.Execute(sql, params);

  std::optional<User> created_user = std::nullopt;
  query_result->First([&created_user](const IRow& row) {
    created_user = UserMapper{}.Map(row);
    });

  if (!created_user) throw std::runtime_error("Failed to insert user.");

  tx.Commit();
  return std::move(*created_user);
}

void PqxxUserRepository::Update(const User& user) {
  auto tx = Transaction{ std::move(connection_pool_obs_->Acquire()) };

  const std::string sql = R"(
    UPDATE users 
    SET login = $2, tag = $3, password_hash = $4, public_key = $5, role = $6 
    WHERE id = $1
  )";

  const auto params = QueryParams{}
    .BindParam(user.GetId().ToString())
    .BindParam(user.GetLogin())
    .BindParam(user.GetTag().ToString())
    .BindParam(user.GetPasswordHash())
    .BindParam(user.GetPublicKey())
    .BindParam(std::string{ user.GetRoleStr() });

  tx.Execute(sql, params);
  tx.Commit();
}

void PqxxUserRepository::DeleteById(const UserId& id) {
  auto tx = Transaction{ std::move(connection_pool_obs_->Acquire()) };

  tx.Execute("DELETE FROM users WHERE id = $1", QueryParams{}.BindParam(id.ToString()));
  tx.Commit();
}