#ifndef I_CONNECTION_POOL_H
#define I_CONNECTION_POOL_H

class PooledConnection;
class IConnection;

#include <memory>

/**
 * @brief Interface for a thread-safe connection pool.
 * * Manages a set of active database connections to optimize performance
 * across highly concurrent network requests.
 */
class IConnectionPool {
public:
  virtual ~IConnectionPool() = default;

  /**
   * @brief Retrieves an available connection from the pool.
   * @return A safely wrapped PooledConnection object.
   */
  [[nodiscard]] virtual PooledConnection Acquire() = 0;

  /**
   * @brief Returns an active connection back to the available pool.
   * * Normally, this is called automatically by the PooledConnection destructor.
   * @param connection The raw connection pointer being relinquished.
   */
  virtual void Return(std::unique_ptr<IConnection> connection) = 0;
};

#endif // I_CONNECTION_POOL_H