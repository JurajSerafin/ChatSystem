#include "Database/Postgres/pqxx_connection.h"

#include "TypeLibHelpers/overload.h"

#include "Database/exceptions.h"

#include <format>
#include <iterator>

#include <pqxx/pqxx>


void PqxxConnection::BeginTransaction() {
  if (transaction_.has_value()) {
    throw TransactionException("A transaction is already active on this connection.");
  }
  transaction_.emplace(connection_);
}

void PqxxConnection::CommitTransaction() {
  if (!transaction_) {
    throw TransactionException("No active transaction to commit.");
  }

  transaction_->commit();
  transaction_.reset();
}

void PqxxConnection::RollbackTransaction() {
  if (!transaction_) {
    throw TransactionException("No active transaction to rollback.");
  }

  transaction_->abort();
  transaction_.reset();
}

bool PqxxConnection::IsAlive() const {
  return connection_.is_open();
}

std::string PqxxConnection::FormatChronoTimePointToPostgres(std::chrono::system_clock::time_point tp) const {
  return std::format("{:%F %T}", tp);
}

std::unique_ptr<PqxxResultSet> PqxxConnection::ExecuteDynamicTransaction(pqxx::transaction_base& transaction, std::string_view query, const pqxx::params& params) {
  return std::make_unique<PqxxResultSet>(transaction.exec(query, params));
}

std::unique_ptr<PqxxResultSet> PqxxConnection::ExecuteTransaction(pqxx::transaction_base& transaction, std::string_view query) {
  return std::make_unique<PqxxResultSet>(transaction.exec(query));
}

std::unique_ptr<IResultSet> PqxxConnection::Execute(std::string_view query) {
  if (transaction_.has_value()) {
    return ExecuteTransaction(*transaction_, query);
  }

  pqxx::work temp_transaction{ connection_ };

  auto transaction_result = ExecuteTransaction(temp_transaction, query);

  temp_transaction.commit();

  return transaction_result;
}

std::unique_ptr<IResultSet> PqxxConnection::Execute(std::string_view query, const QueryParams& params) {

  pqxx::params pqxx_params;

  pqxx_params.reserve(std::distance(params.cbegin(), params.cend()));

  for (auto&& param : params) {
    std::visit(overload{
      [&](std::nullptr_t) { pqxx_params.append(); },
      [&](const std::chrono::system_clock::time_point& paramVal) {
        pqxx_params.append(FormatChronoTimePointToPostgres(paramVal));
      },
      [&](auto&& paramVal) {pqxx_params.append(paramVal); }
      }, param);
  }

  if (transaction_.has_value()) {
    return ExecuteDynamicTransaction(*transaction_, query, pqxx_params);
  }

  pqxx::work temp_transaction{connection_};

  auto transaction_result = ExecuteDynamicTransaction(temp_transaction, query, pqxx_params);

  temp_transaction.commit();

  return transaction_result;
}