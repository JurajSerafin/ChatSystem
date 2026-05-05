#ifndef I_CONNECTION_H
#define I_CONNECTION_H

#include "i_result_set.h"
#include "query_params.h"

#include <memory>
#include <string_view>

class IConnection {
public:
  [[nodiscard]] virtual std::unique_ptr<IResultSet> Execute(std::string_view query, const QueryParams& params) = 0;

  [[nodiscard]] virtual std::unique_ptr<IResultSet> Execute(std::string_view query) = 0;

  virtual void BeginTransaction() = 0;

  virtual void CommitTransaction() = 0;

  virtual void RollbackTransaction() = 0;

  [[nodiscard]] virtual bool IsAlive() const = 0;

  virtual ~IConnection() = default;
};


#endif // I_CONNECTION_H