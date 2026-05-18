#ifndef I_LOCAL_DATABASE_H
#define I_LOCAL_DATABASE_H

#include "User/Roles/user_role_variant.h"
#include "local_db_models.h"

#include <optional>
#include <vector>
#include <string_view>

/**
 * @brief Low-level interface for interacting with the local cache database.
 * * Provides direct CRUD operations for cached  database models.
 * This interface abstracts the raw database execution from the repository layer.
 */
class ILocalDatabase {
public:
  virtual ~ILocalDatabase() = default;

  /**
   * @brief Saves the active user's session and identity to the local database.
   * @param identity The identity object containing the session token.
   */
  virtual void SaveIdentity(const CachedIdentity& identity) = 0;

  /**
   * @brief Retrieves the actively logged-in user's identity and session token.
   * @return An optional containing the identity if a session exists, std::nullopt otherwise.
   */
  virtual std::optional<CachedIdentity> LoadIdentity() = 0;

  /**
   * @brief Wipes the active user's identity and session from the database.
   */
  virtual void ClearIdentity() = 0;

  /**
   * @brief Inserts or updates a user profile in the local cache.
   * @param user The user profile to save.
   */
  virtual void UpsertUser(const CachedUser& user) = 0;

  /**
   * @brief Retrieves a user profile by their unique ID.
   * @param userId The ID of the user to look up.
   * @return An optional containing the user if found, std::nullopt otherwise.
   */
  virtual std::optional<CachedUser> LoadUser(std::string_view userId) = 0;

  /**
   * @brief Retrieves a user profile matching a specific login or tag.
   * @param login The exact login username.
   * @param tag The exact user tag.
   * @return An optional containing the user if found, std::nullopt otherwise.
   */
  virtual std::optional<CachedUser> LoadUserByLoginOrTag(std::string_view login, std::string_view tag) = 0;

  /**
   * @brief Inserts or updates chat room metadata in the local cache.
   * @param chat The chat metadata to save.
   */
  virtual void UpsertChat(const CachedChat& chat) = 0;

  /**
   * @brief Retrieves chat room metadata by its unique ID.
   * @param chatId The ID of the chat to look up.
   * @return An optional containing the chat metadata if found, std::nullopt otherwise.
   */
  virtual std::optional<CachedChat> LoadChat(std::string_view chatId) = 0;

  /**
   * @brief Retrieves a paginated list of chats sorted by most recent activity.
   * @param limit The maximum number of chats to return.
   * @param offset The pagination offset.
   * @return A vector of cached chats.
   */
  virtual std::vector<CachedChat> LoadChats(std::size_t limit, std::size_t offset) = 0;

  /**
   * @brief Links a user to a chat room in the junction table with a specific role.
   * @param userId The ID of the user joining the chat.
   * @param chatId The ID of the chat room.
   * @param chatRole The role assigned to the user within this chat.
   */
  virtual void AddUserToChat(std::string_view userId, std::string_view chatId, const UserRoleVariant& chatRole) = 0;

  /**
   * @brief Updates an existing user's role within a specific chat room.
   * @param userId The ID of the user.
   * @param chatId The ID of the chat room.
   * @param chatRole The new role to assign.
   */
  virtual void SetUserChatRole(std::string_view userId, std::string_view chatId, const UserRoleVariant& chatRole) = 0;

  /**
   * @brief Retrieves a user's specific role within a chat room.
   * @param userId The ID of the user.
   * @param chatId The ID of the chat room.
   * @return An optional containing the role if the user is in the chat, std::nullopt otherwise.
   */
  virtual std::optional<UserRoleVariant> GetUserChatRole(std::string_view userId, std::string_view chatId) = 0;

  /**
   * @brief Retrieves all user IDs belonging to a specific chat room.
   * @param chatId The ID of the chat room.
   * @return A vector of user ID strings.
   */
  virtual std::vector<std::string> GetChatParticipantIds(std::string_view chatId) = 0;

  /**
   * @brief Removes a message entirely from the local cache.
   * @param messageId The ID of the message to delete.
   */
  virtual void DeleteMessage(std::string_view messageId) = 0;

  /**
   * @brief Removes a user profile from the local cache.
   * @param userId The ID of the user to delete.
   */
  virtual void DeleteUser(std::string_view userId) = 0;

  /**
   * @brief Removes a chat room and all associated metadata from the local cache.
   * @param chatId The ID of the chat to delete.
   */
  virtual void DeleteChat(std::string_view chatId) = 0;

  /**
   * @brief Removes a user from a specific chat room (deletes the junction table link).
   * @param userId The ID of the user to remove.
   * @param chatId The ID of the chat room.
   */
  virtual void DeleteUserFromChat(std::string_view userId, std::string_view chatId) = 0;

  /**
   * @brief Saves a fully decrypted message to the local cache.
   * @param message The message object to store.
   */
  virtual void SaveMessageForChat(const CachedMessage& message) = 0;

  /**
   * @brief Retrieves a paginated list of messages for a specific chat room.
   * @param chatId The ID of the chat room.
   * @param limit The maximum number of messages to return.
   * @param offset The pagination offset.
   * @return A vector of decrypted cached messages.
   */
  virtual std::vector<CachedMessage> LoadMessagesForChat(std::string_view chatId, std::size_t limit, std::size_t offset) = 0;

  /**
   * @brief Marks a specific locally cached message as read.
   * @param messageId The ID of the message to update.
   */
  virtual void MarkMessageAsRead(std::string_view messageId) = 0;

  /**
   * @brief Searches the local message cache for plaintext matching a specific keyword.
   * @param chatId The ID of the chat room to search within.
   * @param keyword The plaintext keyword to search for.
   * @return A vector of messages matching the search criteria.
   */
  virtual std::vector<CachedMessage> SearchMessages(std::string_view chatId, std::string_view keyword) = 0;

  /**
   * @brief Prunes old messages from the cache to save local disk space.
   * @param olderThanTimeStamp The threshold timestamp; messages older than this are deleted.
   */
  virtual void EvictOldMessages(std::chrono::system_clock::time_point olderThanTimeStamp) = 0;

  /**
   * @brief Wipes the entire local database clean.
   * * Drops or truncates all tables. Intended for a hard logout or factory reset.
   */
  virtual void NukeEntireCache() = 0;
};

#endif // I_LOCAL_DATABASE_H