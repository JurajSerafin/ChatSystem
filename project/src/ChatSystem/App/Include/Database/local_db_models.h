#ifndef LOCAL_DB_MODELS_H
#define LOCAL_DB_MODELS_H

#include <string>
#include <optional>
#include <chrono>

/**
 * @brief Represents the actively logged-in user's session and identity.
 * * This model is stored securely and indicates who is currently using the application,
 * holding the session token required for authenticated network requests.
 */
struct CachedIdentity {
  /** @brief The user's login username. */
  std::string login;

  /** @brief The unique identifier of the user. */
  std::string id;

  /** @brief The user's unique display tag or handle. */
  std::string tag;

  /** @brief The active network session token. */
  std::string session_token;
};

/**
 * @brief Represents a user profile cached in the local database.
 */
struct CachedUser {
  /** @brief The user's login username. */
  std::string login;

  /** @brief The unique identifier of the user. */
  std::string id;

  /** @brief The user's unique display tag or handle. */
  std::string tag;

  /** @brief The user's public key, used to wrap symmetric E2EE keys. */
  std::string public_key;

  /** @brief Timestamp of when this profile was last synced to the local cache. */
  std::chrono::system_clock::time_point cached_at;

  /** @brief Flag indicating if the user has been soft-deleted. */
  bool is_deleted;
};

/**
 * @brief Represents a chat room's metadata cached in the local database.
 */
struct CachedChat {
  /** @brief The unique identifier of the chat room. */
  std::string id;

  /** @brief The ID of the most recent message in the chat, if any exist. */
  std::optional<std::string> last_message_id;

  /** @brief The assigned name of the chat room (typically null for 1-on-1 chats). */
  std::optional<std::string> name;

  /** @brief Timestamp of the last activity (message sent/received) in this chat. */
  std::chrono::system_clock::time_point last_activity_at;

  /** @brief Timestamp of when this chat metadata was last synced. */
  std::chrono::system_clock::time_point cached_at;

  /** @brief Flag indicating if the chat has been soft-deleted locally. */
  bool is_deleted = false;
};

/**
 * @brief Represents a fully decrypted message cached in the local database.
 */
struct CachedMessage {
  /** @brief The unique identifier of the message. */
  std::string id;

  /** @brief The unique identifier of the user who sent the message. */
  std::string sender_id;

  /** @brief The unique identifier of the chat room this message belongs to. */
  std::string chat_id;

  /** @brief The decrypted, raw text of the message. */
  std::string plaintext;

  /** @brief The type descriptor of the message (e.g., "TEXT", "IMAGE", etc.). */
  std::string type;

  /** @brief Timestamp of when the message was originally created on the server. */
  std::chrono::system_clock::time_point created_at;

  /** @brief Flag indicating if the active user has read this message. */
  bool is_read;

  /** @brief Flag indicating if the message was successfully delivered. */
  bool is_delivered;

  /** @brief Flag indicating if the message was soft-deleted locally. */
  bool is_deleted = false;
};

#endif // LOCAL_DB_MODELS_H