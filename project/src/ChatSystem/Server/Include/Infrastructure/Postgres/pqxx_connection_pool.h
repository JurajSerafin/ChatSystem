#ifndef PQXX_CONNECTION_POOL_H
#define PQXX_CONNECTION_POOL_H

#include <Database/i_connection_pool.h>
#include <Database/pooled_connection.h>
#include <Database/i_connection.h>
#include <Database/exceptions.h>

#include <pqxx/pqxx>

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "pqxx_connection.h"

class PqxxConnectionPool : public IConnectionPool {
public:
  [[nodiscard]] PooledConnection Acquire() override;

  void Return(std::unique_ptr<IConnection> connection) override;

  void Init();

private:
  constexpr static auto kDefaultAcquireTimeout = std::chrono::milliseconds(200);

  constexpr static std::size_t kConnectionCount = 50;

  const std::chrono::milliseconds acquire_timeout_;

  std::deque<std::unique_ptr<IConnection>> free_connections_;

  std::mutex mutex_;

  std::condition_variable cv_;

};

#endif // PQXX_CONNECTION_POOL_H