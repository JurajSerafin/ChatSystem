#ifndef I_CHAT_REPOSITORY_H
#define I_CHAT_REPOSITORY_H

#include <Chat/chat.h>
#include <Chat/chat_id.h>
#include <User/user_id.h>
#include <optional>
#include <vector>

/**
 * @brief Server-side repository interface for managing chat room entities.
 * * Handles persisting chat metadata and managing the many-to-many junction
 * relationships between chat rooms and their participating users.
 */
class IChatRepository {
public:
  virtual ~IChatRepository() = default;

  /**
   * @brief Adds a newly created chat room to the database.
   * @param chat The chat domain entity to save.
   */
  virtual void Add(const Chat& chat) = 0;

  /**
   * @brief Retrieves a specific chat room by its unique identifier.
   * @param id The ID of the chat room.
   * @return An optional containing the chat if found, std::nullopt otherwise.
   */
  virtual std::optional<Chat> FindById(const ChatId& id) = 0;

  /**
   * @brief Retrieves a paginated list of chat rooms a specific user is a part of.
   * @param userId The ID of the participating user.
   * @param limit The maximum number of chats to return.
   * @param offset The pagination offset.
   * @return A vector of chat entities sorted by recent activity.
   */
  virtual std::vector<Chat> FindByUserId(const UserId& userId, std::size_t limit, std::size_t offset) = 0;

  /**
   * @brief Adds a user to a specific chat room in the junction table.
   * @param chatId The target chat room ID.
   * @param userId The ID of the user joining the chat.
   */
  virtual void AddParticipant(const ChatId& chatId, const UserId& userId) = 0;

  /**
   * @brief Removes a user's participation link from a specific chat room.
   * @param chatId The target chat room ID.
   * @param userId The ID of the user leaving the chat.
   */
  virtual void RemoveParticipant(const ChatId& chatId, const UserId& userId) = 0;

  /**
   * @brief Verifies if a user is currently an active member of a specific chat room.
   * @param chatId The chat room ID.
   * @param userId The user ID to check.
   * @return True if the user is a participant, false otherwise.
   */
  virtual bool IsParticipant(const ChatId& chatId, const UserId& userId) = 0;

  /**
   * @brief Denormalization helper to update a chat's last active message reference.
   * * Used to rapidly sort the user's inbox without executing complex joins.
   * @param chatId The chat room ID to update.
   * @param message The latest message that was sent to the chat.
   */
  virtual void UpdateLastMessage(const ChatId& chatId, const Message& message) = 0;

  /**
   * @brief Permanently deletes a chat room and cascades deletions to its participants and messages.
   * @param id The ID of the chat to delete.
   */
  virtual void DeleteById(const ChatId& id) = 0;
};

#endif // I_CHAT_REPOSITORY_H