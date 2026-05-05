#ifndef PQXX_CONNECTION_H
#define PQXX_CONNECTION_H


#include "pqxx_result_set.h"

#include <memory>
#include <string_view>
#include <chrono>

#include <Database/i_connection.h>

#include <pqxx/pqxx>
#include <optional>

class PqxxConnection : public IConnection {
public:

  explicit PqxxConnection(const std::string& connection_uri)
    : connection_(connection_uri) {
  }

  [[nodiscard]] std::unique_ptr<IResultSet> Execute(std::string_view query, const QueryParams& params) override;

  [[nodiscard]] std::unique_ptr<IResultSet> Execute(std::string_view query) override;

  void BeginTransaction() override;

  void CommitTransaction() override;

  void RollbackTransaction() override;

  [[nodiscard]] bool IsAlive() const override;

private:
  [[nodiscard]] std::unique_ptr<PqxxResultSet> ExecuteDynamicTransaction(pqxx::transaction_base& transaction, std::string_view query, const pqxx::params& params);
  [[nodiscard]] std::unique_ptr<PqxxResultSet> ExecuteTransaction(pqxx::transaction_base& transaction, std::string_view query);

  [[nodiscard]] std::string FormatChronoTimePointToPostgres(std::chrono::system_clock::time_point tp) const;

  pqxx::connection connection_;
  std::optional<pqxx::work> transaction_;

};


#endif // PQXX_CONNECTION_H