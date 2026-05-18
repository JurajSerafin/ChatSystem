#ifndef PQXX_CONNECTION_FACTORY_H
#define PQXX_CONNECTION_FACTORY_H

#include "pqxx_connection.h"

#include <memory>

/**
 * @brief Simple internal DTO holding the extracted PostgreSQL connection parameters.
 */
struct ConfigParams {
  const char* user;
  const char* password;
  const char* name;
  const char* port;
  const char* host;

  /** @brief Validates that all required environment variables were successfully loaded. */
  [[nodiscard]] constexpr bool AllSet() const;
};

constexpr bool ConfigParams::AllSet() const {
  return user && password && name && port && host;
}

/**
 * @brief Factory for instantiating PostgreSQL database connections.
 * * Reads secure database credentials directly from system environment variables,
 * enforcing a secure architecture.
 */
class PqxxConnectionFactory {
public:
  /**
   * @brief Reads the environment, constructs the URI, and establishes a new PqxxConnection.
   * @return A unique pointer to the newly allocated connection.
   */
  [[nodiscard]] static std::unique_ptr<PqxxConnection> Create();

private:
  /** @brief Extracts credentials via std::getenv(). */
  [[nodiscard]] static ConfigParams LoadDbConfig();

  /** @brief Throws a descriptive exception if required environment variables are missing. */
  static void HandleUnsetConfigVar();

  /** @brief Formats the ConfigParams into a valid libpqxx connection URI string. */
  static std::string ConstructConnectionUri(ConfigParams dbConfig);

  static void HandleClosedDbConnection();

  // --- Environment Variable Keys ---
  constexpr static auto kDbUserVarName = "DB_USER";
  constexpr static auto kDbPasswordVarName = "DB_PASSWORD";
  constexpr static auto kDbNameVarName = "DB_NAME";
  constexpr static auto kDbPortVarName = "DB_PORT";
  constexpr static auto kDbHostVarName = "DB_HOST";
};

#endif // PQXX_CONNECTION_FACTORY_H