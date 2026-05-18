#ifndef I_LOCAL_CHAT_REPOSITORY_H
#define I_LOCAL_CHAT_REPOSITORY_H

#include <vector>
#include <string_view>

#include "Database/local_db_models.h"

/**
 * @brief Repository interface for managing cached chat rooms and their participants.
 * * Abstracts the underlying local database to handle chat metadata and
 * the many-to-many junction relationships between users and chats.
 */
class ILocalChatRepository {
public:
  virtual ~ILocalChatRepository() = default;

  /**
   * @brief Retrieves all cached chat rooms available to the user.
   * @return A vector of CachedChat objects, typically sorted by recent activity.
   */
  virtual std::vector<CachedChat> FindAll() = 0;

  /**
   * @brief Retrieves a specific chat room by its unique identifier.
   * @param chatId The ID of the chat room to locate.
   * @return An optional containing the chat if found, std::nullopt otherwise.
   */
  virtual std::optional<CachedChat> FindById(std::string_view chatId) = 0;

  /**
   * @brief Inserts a new chat room or updates an existing one in the local cache.
   * @param chat The chat metadata object to save.
   */
  virtual void Upsert(const CachedChat& chat) = 0;

  /**
   * @brief Updates the most recent message ID and activity timestamp for a chat.
   * @param chatId The ID of the chat to update.
   * @param messageId The ID of the latest message.
   * @param lastActivityMs The epoch timestamp (in milliseconds) of the latest activity.
   */
  virtual void UpdateLastMessage(std::string_view chatId, std::string_view messageId, int64_t lastActivityMs) = 0;

  /**
   * @brief Deletes a chat room and cascades deletions to its local messages and participant links.
   * @param chatId The ID of the chat to delete.
   */
  virtual void Delete(std::string_view chatId) = 0;

  /**
   * @brief Links a user to a specific chat room with an assigned role.
   * @param userId The ID of the user to add.
   * @param chatId The ID of the chat room.
   * @param role The assigned role of the user in the chat.
   */
  virtual void AddParticipant(std::string_view userId, std::string_view chatId, const UserRoleVariant& role) = 0;

  /**
   * @brief Retrieves the IDs of all users participating in a specific chat room.
   * @param chatId The ID of the chat room.
   * @return A vector of user ID strings.
   */
  virtual std::vector<std::string> GetParticipantIds(std::string_view chatId) = 0;

  /**
   * @brief Removes a user's participation link from a specific chat room.
   * @param userId The ID of the user to remove.
   * @param chatId The ID of the chat room.
   */
  virtual void RemoveParticipant(std::string_view userId, std::string_view chatId) = 0;
};

#endif // I_LOCAL_CHAT_REPOSITORY_H