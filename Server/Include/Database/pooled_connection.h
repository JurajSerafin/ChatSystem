#ifndef POOLED_CONNECTION_H
#define POOLED_CONNECTION_H

#include "i_connection.h"
#include "i_connection_pool.h"
#include "i_result_set.h"
#include "query_params.h"

#include <memory>
#include <utility>

/**
 * @brief RAII wrapper for a borrowed database connection.
 * * Ensures that a connection is automatically returned to its parent IConnectionPool
 * as soon as this object goes out of scope, preventing connection leaks.
 */
class PooledConnection {
public:
  std::unique_ptr<IResultSet> Execute(std::string_view query, const QueryParams& params);
  std::unique_ptr<IResultSet> Execute(std::string_view query);
  [[nodiscard]] bool IsAlive() const noexcept;
  [[nodiscard]] IConnection& GetConnection();

  PooledConnection() = delete;
  PooledConnection(const PooledConnection&) = delete;
  PooledConnection& operator=(const PooledConnection&) = delete;

  /** @brief Safely transfers ownership of the connection to a new wrapper. */
  PooledConnection(PooledConnection&& other) noexcept;
  PooledConnection& operator=(PooledConnection&& other) noexcept;

  /**
   * @brief Initializes the wrapper with a raw connection and a pointer to the parent pool.
   */
  PooledConnection(std::unique_ptr<IConnection> connectionPtr, IConnectionPool* connectionPoolPtr);

  /**
   * @brief Destructor. Automatically returns the raw connection to the pool.
   */
  ~PooledConnection();

private:
  std::unique_ptr<IConnection> connection_;
  IConnectionPool* connection_pool_obs_;

  /** @brief Safely returns the held connection to the observer pool. */
  void ReturnExistingConnectionToPool() noexcept;
};

inline PooledConnection::PooledConnection(PooledConnection&& other) noexcept 
  : connection_(std::move(other.connection_)), connection_pool_obs_(other.connection_pool_obs_){
  other.connection_pool_obs_ = nullptr;
}

inline PooledConnection::PooledConnection(std::unique_ptr<IConnection> connectionPtr, IConnectionPool* connectionPoolPtr)
  : connection_(std::move(connectionPtr)), connection_pool_obs_(connectionPoolPtr) {
}

inline PooledConnection& PooledConnection::operator=(PooledConnection&& other) noexcept {
  if (this != &other) {
      ReturnExistingConnectionToPool();

      connection_ = std::move(other.connection_);
      connection_pool_obs_ = other.connection_pool_obs_;

      other.connection_pool_obs_ = nullptr;
  }

  return *this;
}

inline PooledConnection::~PooledConnection() {
  ReturnExistingConnectionToPool();
}

inline void PooledConnection::ReturnExistingConnectionToPool() noexcept {
  if (connection_ && connection_pool_obs_) {
      connection_pool_obs_->Return(std::move(connection_));
  }
}

inline IConnection& PooledConnection::GetConnection() {
  return *connection_;
}

inline std::unique_ptr<IResultSet> PooledConnection::Execute(std::string_view query, const QueryParams& params) {
  return connection_->Execute(query, params);
}

inline std::unique_ptr<IResultSet> PooledConnection::Execute(std::string_view query) {
  return connection_->Execute(query);
}

inline bool PooledConnection::IsAlive() const noexcept {
  return connection_ != nullptr && connection_->IsAlive();
}

#endif // POOLED_CONNECTION_H