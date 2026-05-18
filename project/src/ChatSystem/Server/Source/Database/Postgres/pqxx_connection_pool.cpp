#include "Database/Postgres/pqxx_connection_pool.h"
#include "Database/Postgres/pqxx_connection_factory.h"
#include "Database/Postgres/pqxx_connection.h"


#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <cstddef>

PooledConnection PqxxConnectionPool::Acquire() {
  const auto deadline = std::chrono::system_clock::now() + acquire_timeout_;

  std::unique_lock lock{ mutex_ };

  while (free_connections_.empty())
  {
    auto status = cv_.wait_until(lock, deadline);

    if (status == std::cv_status::timeout) {
      throw ConnectionException{ "Failed to acquire a free database connection within a given time." };
    }
  }

  auto free_connection_ptr = std::move(free_connections_.front());
  free_connections_.pop_front();

  lock.unlock();

  return PooledConnection{std::move(free_connection_ptr), this};
}

void PqxxConnectionPool::Return(std::unique_ptr<IConnection> connection) {
  std::unique_lock lock{ mutex_ };

  free_connections_.emplace_back(std::move(connection));

  cv_.notify_one();

  lock.unlock();
}

void PqxxConnectionPool::Init() {
  for (std::size_t conn_i = 0; conn_i < kConnectionCount; ++conn_i) {
    free_connections_.emplace_back(PqxxConnectionFactory::Create());
  }
}