#include <Infrastructure/Postgres/Repositories/pqxx_session_repository.h>
#include <Infrastructure/Postgres/Mappers/session_mapper.h>
#include <Database/query_params.h>
#include <Database/transaction.h>
#include <stdexcept>

PqxxSessionRepository::PqxxSessionRepository(IConnectionPool* connection_pool_obs_obs) : connection_pool_obs_(connection_pool_obs_obs) {
  if (!connection_pool_obs_) {
    throw std::invalid_argument("PqxxSessionRepository requires a valid IConnectionPool pointer.");
  }
}

std::optional<Session> PqxxSessionRepository::TryFetchSession(std::string_view query, const QueryParams& params, Transaction&& tx) {
  const auto query_result = tx.Execute(query, params);

  if (query_result->IsEmpty()) {
    return std::nullopt;
  }

  std::optional<Session> fetched_session = std::nullopt;

  query_result->First([&fetched_session](const IRow& row) {
      fetched_session = SessionMapper{}.Map(row);
  });

  return fetched_session;
}

std::vector<Session> PqxxSessionRepository::TryFetchSessions(std::string_view query, const QueryParams& params, Transaction&& tx) {
  const auto query_result = tx.Execute(query, params);

  std::vector<Session> fetched_sessions;

  if (query_result->IsEmpty()) {
    return fetched_sessions;
  }

  fetched_sessions.reserve(query_result->GetSize());
  SessionMapper mapper{};

  query_result->ForEach([&fetched_sessions, &mapper](const IRow& row) {
    fetched_sessions.emplace_back(mapper.Map(row));
  });

  return fetched_sessions;
}

std::optional<Session> PqxxSessionRepository::FindByToken(const std::string& token) {
  return TryFetchSession(
    "SELECT * FROM sessions WHERE token = $1",
    QueryParams{}.BindParam(token),
    Transaction{ std::move(connection_pool_obs_->Acquire()) }
  );
}

std::vector<Session> PqxxSessionRepository::FindByUserId(UserId userId) {
  return TryFetchSessions(
    "SELECT * FROM sessions WHERE user_id = $1",
    QueryParams{}.BindParam(std::move(userId.ToString())),
    Transaction{ std::move(connection_pool_obs_->Acquire()) }
  );
}

Session PqxxSessionRepository::Create(const Session& session) {
  auto tx = Transaction{ std::move(connection_pool_obs_->Acquire()) };

  const std::string query = R"(
    INSERT INTO sessions (id, user_id, token, expires_at) 
    VALUES ($1, $2, $3, $4) 
    RETURNING *
  )";

  auto params = QueryParams{}
    .BindParam(session.GetId().ToString())
    .BindParam(session.GetUserId().ToString())
    .BindParam(session.GetToken())
    .BindParam(session.ExpiresAt());

  const auto query_result = tx.Execute(query, params);

  std::optional<Session> created_session = std::nullopt;

  query_result->First([&created_session](const IRow& row) {
    created_session = SessionMapper{}.Map(row);
    });

  if (!created_session) {
    throw std::runtime_error("Failed to insert session: No returning row.");
  }

  tx.Commit();

  return std::move(*created_session);
}

void PqxxSessionRepository::DeleteByToken(const std::string& token) {
  auto tx = Transaction{ std::move(connection_pool_obs_->Acquire()) };

  const std::string query = "DELETE FROM sessions WHERE token = $1";

  tx.Execute(query, QueryParams{}.BindParam(token));

  tx.Commit();
}

void PqxxSessionRepository::DeleteAllForUser(UserId userId) {
  auto tx = Transaction{ std::move(connection_pool_obs_->Acquire()) };

  const std::string query = "DELETE FROM sessions WHERE user_id = $1";

  tx.Execute(query, QueryParams{}.BindParam(std::move(userId.ToString())));

  tx.Commit();
}
