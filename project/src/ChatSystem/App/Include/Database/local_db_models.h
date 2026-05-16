#ifndef LOCAL_DB_MODELS_H
#define LOCAL_DB_MODELS_H

#include <string>
#include <optional>
#include <chrono>

struct CachedIdentity {
  std::string login;
  std::string id;
  std::string tag;
  std::string session_token;
};

struct CachedUser {
  std::string login;
  std::string id;
  std::string tag;
  std::string public_key;

  std::chrono::system_clock::time_point cached_at;

  bool is_deleted;

};

struct CachedChat {
  std::string id;

  std::optional<std::string> last_message_id;
  std::optional<std::string> name;

  std::chrono::system_clock::time_point last_activity_at;
  std::chrono::system_clock::time_point cached_at;

  bool is_deleted = false;
};

struct CachedMessage {
  std::string id;
  std::string sender_id;
  std::string chat_id;
  std::string plaintext;
  std::string type;

  std::chrono::system_clock::time_point created_at;

  bool is_read;
  bool is_delivered;

  bool is_deleted = false;
};

#endif // LOCAL_DB_MODELS_H