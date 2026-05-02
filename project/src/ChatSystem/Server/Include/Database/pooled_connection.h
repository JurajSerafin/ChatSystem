#ifndef POOLED_CONNECTION_H
#define POOLED_CONNECTION_H

#include "i_connection.h"
#include "i_connection_pool.h"
#include "i_result_set.h"
#include "query_params.h"

#include <memory>

class PooledConnection {
public:
  [[nodiscard]] std::unique_ptr<IResultSet> Execute(std::string_view query, const QueryParams& params);

  [[nodiscard]] std::unique_ptr<IResultSet> Execute(std::string_view query);

  [[nodiscard]] IConnection& GetConnection();

  PooledConnection() = delete;

  PooledConnection(const PooledConnection&) = delete;

  PooledConnection(PooledConnection&&) = default;

  PooledConnection(IConnection* connectionPtr, IConnectionPool* connectionPoolPtr);

  PooledConnection(IConnection&& connectionPtr, IConnectionPool&& connectionPoolPtr);

  PooledConnection& operator=(const PooledConnection&) = delete;
  
  PooledConnection& operator=(PooledConnection&& other) noexcept;

  ~PooledConnection();

private:
  IConnection* connection_obs_;
  IConnectionPool* connection_pool_obs_;
};


inline PooledConnection::PooledConnection(IConnection* connectionPtr, IConnectionPool* connectionPoolPtr) 
  : connection_obs_(connectionPtr), connection_pool_obs_(connectionPoolPtr) {}

inline PooledConnection::PooledConnection(IConnection&& connectionPtr, IConnectionPool&& connectionPoolPtr) 
  : connection_obs_(&connectionPtr), connection_pool_obs_(&connectionPoolPtr) {}

inline PooledConnection& PooledConnection::operator=(PooledConnection&& other) noexcept {
    if (this != &other) {
        this->connection_obs_ = other.connection_obs_;
        this->connection_pool_obs_ = other.connection_pool_obs_;

        other.connection_obs_ = nullptr;
        other.connection_pool_obs_ = nullptr;
    }

    return *this;
}

inline PooledConnection::~PooledConnection() {
    connection_pool_obs_->Return(std::move(*connection_obs_));
}

inline IConnection& PooledConnection::GetConnection() {
  return *connection_obs_;
}

inline std::unique_ptr<IResultSet> PooledConnection::Execute(std::string_view query, const QueryParams& params) {
  return connection_obs_->Execute(query, params);
}

inline std::unique_ptr<IResultSet> PooledConnection::Execute(std::string_view query) {
  return connection_obs_->Execute(query);
}

#endif // POOLED_CONNECTION_H