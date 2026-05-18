#ifndef PQXX_CONNECTION_FACTORY_H
#define PQXX_CONNECTION_FACTORY_H


#include "pqxx_connection.h"

#include <memory>

struct ConfigParams {
  const char* user;

  const char* password;

  const char* name;

  const char* port;

  const char* host;

  [[nodiscard]] constexpr bool AllSet() const;
};

constexpr bool ConfigParams::AllSet() const {
  return user
    && password
    && name
    && port
    && host;
}

class PqxxConnectionFactory {
public:
  [[nodiscard]] static std::unique_ptr<PqxxConnection> Create();


private:
  [[nodiscard]] static ConfigParams LoadDbConfig();

  static void HandleUnsetConfigVar();

  static std::string ConstructConnectionUri(ConfigParams dbConfig);

  static void HandleClosedDbConnection();

  constexpr static auto kDbUserVarName = "DB_USER";

  constexpr static auto kDbPasswordVarName = "DB_PASSWORD";

  constexpr static auto kDbNameVarName = "DB_NAME";

  constexpr static auto kDbPortVarName = "DB_PORT";

  constexpr static auto kDbHostVarName = "DB_HOST";

};

#endif // PQXX_CONNECTION_FACTORY_H