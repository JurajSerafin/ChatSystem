#ifndef I_CONNECTION_POOL_H
#define I_CONNECTION_POOL_H

#include "pooled_connection.h"

class IConnectionPool {
public:
  [[nodiscard]] virtual PooledConnection Acquire() = 0;

  virtual void Return(IConnection&& connection) = 0;
};

#endif // I_CONNECTION_POOL_H