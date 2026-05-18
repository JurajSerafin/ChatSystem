#ifndef PQXX_CONNECTION_POOL_H
#define PQXX_CONNECTION_POOL_H

#include "pqxx_connection.h"

#include <Database/i_connection.h>
#include <Database/i_connection_pool.h>
#include <Database/pooled_connection.h>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>

/**
 * @brief Thread-safe PostgreSQL connection pool.
 * * Pre-allocates a fixed number of physical database connections to
 * reduce request latency.
 * 
 * Uses a mutex and condition variable to safely block and wake up network threads
 * requesting database access under heavy concurrent load.
 */
class PqxxConnectionPool : public IConnectionPool {
public:
  PqxxConnectionPool() = default;

  /**
   * @brief Initializes the pool by spinning up `kConnectionCount` physical connections via the factory.
   */
  void Init();

  [[nodiscard]] PooledConnection Acquire() override;
  void Return(std::unique_ptr<IConnection> connection) override;

private:
  /** @brief Maximum duration a thread will block waiting for a free connection before throwing an error. */
  constexpr static auto kDefaultAcquireTimeout = std::chrono::milliseconds(200);

  /** @brief The total number of physical PostgreSQL connections to keep warm. */
  constexpr static std::size_t kConnectionCount = 50;

  const std::chrono::milliseconds acquire_timeout_{ kDefaultAcquireTimeout };

  /** @brief A double-ended queue storing idle connections ready for checkout. */
  std::deque<std::unique_ptr<IConnection>> free_connections_;

  /** @brief Protects access to the `free_connections_` queue across concurrent network threads. */
  std::mutex mutex_;

  /** @brief Used to efficiently block and notify waiting threads when a connection is returned. */
  std::condition_variable cv_;
};

#endif // PQXX_CONNECTION_POOL_H