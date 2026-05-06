#include "Database/query_params.h"
#include "Database/transaction.h"
#include "User/user.h"

#include <Infrastructure/Postgres/Repositories/pqxx_user_repository.h>
#include <Infrastructure/Postgres/Mappers/user_mapper.h>

#include <string>

std::optional<User> PqxxUserRepository::TryFetchUser(std::string_view query, const QueryParams& params, Transaction&& tx) {
  const auto query_result = tx.Execute(query, params);

  if (query_result->IsEmpty()) {
    return std::nullopt;
  }

  std::optional<User> fetched_user = std::nullopt;

  query_result->First([&fetched_user](const IRow& row) {fetched_user = UserMapper{}.Map(row); });

  return fetched_user;
}

std::vector<User> PqxxUserRepository::TryFetchUsers(std::string_view query, Transaction&& tx) {
  const auto query_result = tx.Execute(query);

  std::vector<User> fetched_users;

  if (query_result->IsEmpty()) {
    return fetched_users;
  }

  UserMapper mapper{};

  query_result->ForEach([&fetched_users, &mapper](const IRow& row) { fetched_users.emplace_back(mapper.Map(row)); });

  return fetched_users;
}

std::optional<User> PqxxUserRepository::FindById(const UserId& id) {
  return TryFetchUser(
    "SELECT * FROM users WHERE id = $1",
    QueryParams{}.BindParam(id.ToString()),
    Transaction{ std::move(connection_pool_obs_->Acquire()) }
  );
}

std::optional<User> FindByLogin(const std::string& login) {
  return TryFetchUser(
    "SELECT * FROM users WHERE login = $1",
    QueryParams{}.BindParam(login),
    Transaction{ std::move(connection_pool_obs_->Acquire()) }
  );
}

std::optional<User> PqxxUserRepository::FindByTag(const std::string& tag) {
  return TryFetchUser(
    "SELECT * FROM users WHERE tag = $1",
    QueryParams{}.BindParam(tag),
    Transaction{ std::move(connection_pool_obs_->Acquire()) }
  );
}

std::vector<User> PqxxUserRepository::Search(const std::string& query) {
  const std::string sql = "SELECT * FROM users WHERE login ILIKE $1 OR tag ILIKE $1 LIMIT 50";

  std::string search_term = "%" + query + "%";

  return TryFetchUsers(
    sql,
    QueryParams{}.BindParam(std::move(search_term)),
    Transaction{ std::move(connection_pool_obs_->Acquire()) }
  );
}

User PqxxUserRepository::Create(const User& user) {
  auto tx = Transaction{ std::move(connection_pool_obs_->Acquire()) };

  const std::string query = R"(
      INSERT INTO users (id, login, tag, created_at) 
      VALUES ($1, $2, $3, $4) 
      RETURNING *
  )";

  auto params = QueryParams{}
    .BindParam(user.GetId().ToString())
    .BindParam(user.GetLogin())
    .BindParam(user.GetTag().ToString())
    .BindParam(user.CreatedAt());

  auto query_result = tx.Execute(query, params);

  std::optional<User> created_user = std::nullopt;

  query_result->First([&created_user](const IRow& row) {
    created_user = UserMapper{}.Map(row);
    });

  if (!created_user) {
    throw std::runtime_error("Failed to insert user: No returning row.");
  }

  tx.Commit();

  return std::move(*created_user);
}

void PqxxUserRepository::Update(const User& user) {
  auto tx = Transaction{ std::move(connection_pool_obs_->Acquire()) };

  const std::string query = R"(
        UPDATE users 
        SET login = $2, tag = $3 
        WHERE id = $1
    )";

  auto params = QueryParams{}
    .BindParam(user.GetId().ToString())
    .BindParam(user.GetLogin())
    .BindParam(user.GetTag().ToString());

  tx.Execute(query, params);

  tx.Commit();
}

void PqxxUserRepository::DeleteById(const UserId& id) {
  auto tx = Transaction{ std::move(connection_pool_obs_->Acquire()) };

  const std::string query = "DELETE FROM users WHERE id = $1";

  tx.Execute(query, QueryParams{}.BindParam(id.ToString()));

  tx.Commit();
}

void PqxxUserRepository::StorePublicKey(const UserId& id, const std::string& publicKey) {
  auto tx = Transaction{ std::move(connection_pool_obs_->Acquire()) };

  const std::string query = "UPDATE users SET public_key = $2 WHERE id = $1";

  auto params = QueryParams{}
    .BindParam(id.ToString())
    .BindParam(publicKey);

  tx.Execute(query, params);

  tx.Commit();
}

std::optional<std::string> PqxxUserRepository::GetPublicKey(const UserId& id) {
  auto tx = Transaction{ std::move(connection_pool_obs_->Acquire()) };

  const std::string query = "SELECT public_key FROM users WHERE id = $1";

  const auto query_result = tx.Execute(query, QueryParams{}.BindParam(id.ToString()));

  if (query_result->IsEmpty()) {
    return std::nullopt;
  }

  std::optional<std::string> public_key = std::nullopt;

  query_result->First([&public_key](const IRow& row) {
    if (!row.IsNull("public_key")) {
      public_key = row.GetString("public_key");
    }
  });

  return public_key;
}
