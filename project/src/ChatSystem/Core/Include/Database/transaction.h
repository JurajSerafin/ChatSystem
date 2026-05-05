#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "i_connection.h"
#include "pooled_connection.h"

#include <memory>

class Transaction {
public:

  void Commit();

  [[nodiscard]] std::unique_ptr<IResultSet> Execute(std::string_view query, const QueryParams& params);

  [[nodiscard]] std::unique_ptr<IResultSet> Execute(std::string_view query);


  explicit Transaction(PooledConnection connection);

  Transaction() = delete;

  Transaction(const Transaction&) = delete;

  Transaction& operator=(const Transaction&) = delete;

  Transaction(Transaction&& other) = default;

  Transaction& operator=(Transaction&& other) noexcept;

  ~Transaction();


private:

  void CleanupExistingTransaction();

  PooledConnection connection_;

  bool is_commited_{ false };
};

inline Transaction::Transaction(PooledConnection connection) : connection_(std::move(connection)) {
    connection_.GetConnection().BeginTransaction();
}

inline Transaction& Transaction::operator=(Transaction&& other) noexcept {
  if (this != &other) {
    CleanupExistingTransaction();

    this->connection_ = std::move(other.connection_);
    this->is_commited_ = other.is_commited_;
  }

  return *this;
}

inline Transaction::~Transaction() {
  CleanupExistingTransaction();
}
inline void Transaction::CleanupExistingTransaction() {
  if (!is_commited_ && connection_.IsAlive()) {
    connection_.GetConnection().RollbackTransaction();
  }
}

inline void Transaction::Commit() {
  if (is_commited_) {
    return;
  }

  connection_.GetConnection().CommitTransaction();

  is_commited_ = true;
}

inline std::unique_ptr<IResultSet> Transaction::Execute(std::string_view query, const QueryParams& params) {
    return connection_.Execute(query, params);
}

inline std::unique_ptr<IResultSet> Transaction::Execute(std::string_view query) {
    return connection_.Execute(query);
}

#endif // TRANSACTION_H
