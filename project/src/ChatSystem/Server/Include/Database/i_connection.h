#ifndef I_CONNECTION_H
#define I_CONNECTION_H

#include "i_result_set.h"
#include "query_params.h"

#include <memory>
#include <string_view>

/**
 * @brief Low-level interface representing a single active database connection.
 * * Abstracts away the specific database system, allowing the repositories to 
 * execute queries generically.
 */
class IConnection {
public:
  virtual ~IConnection() = default;

  /**
   * @brief Executes a parameterized query safely.
   * @param query The query string containing placeholders.
   * @param params The bound parameters matching the query placeholders.
   * @return A unique pointer to the resulting dataset.
   */
  virtual std::unique_ptr<IResultSet> Execute(std::string_view query, const QueryParams& params) = 0;

  /**
   * @brief Executes a raw query without parameters.
   * @param query The exact query string.
   * @return A unique pointer to the resulting dataset.
   */
  virtual std::unique_ptr<IResultSet> Execute(std::string_view query) = 0;

  /** @brief Opens a new database transaction block. */
  virtual void BeginTransaction() = 0;

  /** @brief Commits the active transaction, persisting the changes. */
  virtual void CommitTransaction() = 0;

  /** @brief Rolls back the active transaction, discarding changes. */
  virtual void RollbackTransaction() = 0;

  /**
   * @brief Checks if the physical database connection is still active and healthy.
   * @return True if the connection is alive, false if it has dropped or errored.
   */
  [[nodiscard]] virtual bool IsAlive() const = 0;
};

#endif // I_CONNECTION_H