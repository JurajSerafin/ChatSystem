#ifndef PQXX_CONNECTION_H
#define PQXX_CONNECTION_H

#include "pqxx_result_set.h"

#include <memory>
#include <string_view>
#include <chrono>

#include <Database/i_connection.h>

#include <pqxx/pqxx>
#include <optional>

/**
 * @brief Concrete implementation of a database connection using PostgreSQL and libpqxx.
 * * Wraps the underlying physical `pqxx::connection` and manages active `pqxx::work`
 * transactions, bridging abstract repository queries to actual PostgreSQL execution.
 */
class PqxxConnection : public IConnection {
public:
  /**
   * @brief Establishes a physical connection to the PostgreSQL server.
   * @param connection_uri The formatted PostgreSQL connection string.
   */
  explicit PqxxConnection(const std::string& connection_uri)
    : connection_(connection_uri) {
  }

  std::unique_ptr<IResultSet> Execute(std::string_view query, const QueryParams& params) override;
  std::unique_ptr<IResultSet> Execute(std::string_view query) override;
  void BeginTransaction() override;
  void CommitTransaction() override;
  void RollbackTransaction() override;
  [[nodiscard]] bool IsAlive() const override;

private:
  /** @brief Helper to safely inject dynamic parameters into a libpqxx transaction. */
  [[nodiscard]] std::unique_ptr<PqxxResultSet> ExecuteDynamicTransaction(pqxx::transaction_base& transaction, std::string_view query, const pqxx::params& params);

  /** @brief Helper to execute parameterless static queries. */
  [[nodiscard]] std::unique_ptr<PqxxResultSet> ExecuteTransaction(pqxx::transaction_base& transaction, std::string_view query);

  /** @brief Utility to serialize native C++ time points into Postgres-compatible timestamp strings. */
  [[nodiscard]] std::string FormatChronoTimePointToPostgres(std::chrono::system_clock::time_point tp) const;

  /** @brief The underlying raw libpqxx connection object. */
  pqxx::connection connection_;

  /** @brief Holds the active transaction scope. std::nullopt when no transaction is explicitly running. */
  std::optional<pqxx::work> transaction_;
};

#endif // PQXX_CONNECTION_H