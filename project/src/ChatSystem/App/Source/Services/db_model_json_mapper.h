#ifndef DB_MODEL_JSON_MAPPER_H
#define DB_MODEL_JSON_MAPPER_H

#include "Database/local_db_models.h"
#include <nlohmann/json.hpp>
#include <string>

class DBModelJsonMapper {
public:
  static CachedIdentity ToIdentity(const nlohmann::json& j);

  static CachedUser ToUser(const nlohmann::json& j);

  static CachedChat ToChat(const nlohmann::json& j);

  static CachedMessage ToMessage(const nlohmann::json& j, std::string plaintext);
};

#endif // DB_MODEL_JSON_MAPPER_H