#include "Infrastructure/Postgres/pqxx_connection_factory.h"

#include <Database/exceptions.h>
#include <cstdlib>
#include <format>
#include <pqxx/pqxx>
#include <stdexcept>
#include <string>

ConfigParams PqxxConnectionFactory::LoadDbConfig() {
  return ConfigParams{
    .user = std::getenv(kDbUserVarName),
    .password = std::getenv(kDbPasswordVarName),
    .name = std::getenv(kDbNameVarName),
    .port = std::getenv(kDbPortVarName),
    .host = std::getenv(kDbHostVarName)
  };
}

void PqxxConnectionFactory::HandleUnsetConfigVar() {
  throw ConnectionException(std::format(
    "Database connection failed: Missing required environment variables.\n Ensure {}, {}, {}, {}, and {} are set.",
    kDbUserVarName, kDbPasswordVarName, kDbNameVarName, kDbPortVarName, kDbHostVarName));
}

std::string PqxxConnectionFactory::ConstructConnectionUri(ConfigParams dbConfig) {
  return std::format(
    "postgresql://{}:{}@{}:{}/{}",
    dbConfig.user, dbConfig.password, dbConfig.host, dbConfig.port, dbConfig.name
  );
}

void PqxxConnectionFactory::HandleClosedDbConnection() {
  throw ConnectionException("Failed to open connection to the database.");
}

std::unique_ptr<PqxxConnection> PqxxConnectionFactory::Create() {

  const auto db_config = LoadDbConfig();

  if (!db_config.AllSet()) {
    HandleUnsetConfigVar();
  }

  auto connection = std::make_unique<PqxxConnection>(ConstructConnectionUri(db_config));

  if (!connection->IsAlive()) {
    HandleClosedDbConnection();
  }

  return connection;
}
