#ifndef I_CONNECTION_POOL_H
#define I_CONNECTION_POOL_H

class PooledConnection;

#include <memory>

class IConnectionPool {
public:
  [[nodiscard]] virtual PooledConnection Acquire() = 0;

  virtual void Return(std::unique_ptr<IConnection> connection) = 0;
};

#endif // I_CONNECTION_POOL_H