#ifndef DB_MODEL_JSON_MAPPER_H
#define DB_MODEL_JSON_MAPPER_H

#include "Database/local_db_models.h"
#include <nlohmann/json.hpp>
#include <string>

/**
 * @brief Static utility class for deserializing raw network JSON payloads into C++ domain entities.
 * * Bridges the gap between the server's API contracts and the client's local SQLite models.
 */
class DBModelJsonMapper {
public:
  /**
   * @brief Maps a JSON authentication payload into a local CachedIdentity struct.
   * @param j The JSON response body from login/registration.
   * @return A populated CachedIdentity containing the session token.
   */
  static CachedIdentity ToIdentity(const nlohmann::json& j);

  /**
   * @brief Maps a JSON profile payload into a local CachedUser struct.
   * @param j The JSON response body.
   * @return A populated CachedUser profile.
   */
  static CachedUser ToUser(const nlohmann::json& j);

  /**
   * @brief Maps a JSON chat room payload into a local CachedChat struct.
   * @param j The JSON response body.
   * @return A populated CachedChat metadata object.
   */
  static CachedChat ToChat(const nlohmann::json& j);

  /**
   * @brief Maps a JSON message payload into a local CachedMessage struct.
   * * Requires the manually decrypted plaintext string since the network JSON only holds ciphertext.
   * @param j The JSON response body.
   * @param plaintext The already-decrypted body of the message.
   * @return A populated CachedMessage ready for local storage.
   */
  static CachedMessage ToMessage(const nlohmann::json& j, std::string plaintext);
};

#endif // DB_MODEL_JSON_MAPPER_H