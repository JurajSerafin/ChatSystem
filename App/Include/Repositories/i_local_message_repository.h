#ifndef I_LOCAL_MESSAGE_REPOSITORY_H
#define I_LOCAL_MESSAGE_REPOSITORY_H

#include "Database/local_db_models.h"

/**
 * @brief Repository interface for managing cached messages within chat rooms.
 * * Provides methods for querying, storing, and updating the state of fully
 * decrypted messages stored in the local SQLite database.
 */
class ILocalMessageRepository {
public:
  virtual ~ILocalMessageRepository() = default;

  /**
   * @brief Retrieves a paginated history of messages for a specific chat room.
   * @param chatId The ID of the chat room.
   * @param limit The maximum number of messages to retrieve.
   * @param offset The pagination offset.
   * @return A vector of decrypted messages.
   */
  virtual std::vector<CachedMessage> FindByChatId(std::string_view chatId, std::size_t limit, std::size_t offset) = 0;

  /**
   * @brief Performs a local plaintext search for messages matching specific keywords.
   * @param chatId The ID of the chat room to search within.
   * @param keywords The search query string.
   * @return A vector of messages matching the search criteria.
   */
  virtual std::vector<CachedMessage> Search(std::string_view chatId, std::string_view keywords) = 0;

  /**
   * @brief Inserts a new message or updates an existing one in the local cache.
   * @param message The message object to save.
   */
  virtual void Upsert(const CachedMessage& message) = 0;

  /**
   * @brief Removes a specific message entirely from the local cache.
   * @param messageId The ID of the message to delete.
   */
  virtual void Delete(std::string_view messageId) = 0;

  /**
   * @brief Updates the read status of a locally cached message.
   * @param messageId The ID of the message to mark as read.
   */
  virtual void MarkAsRead(std::string_view messageId) = 0;

  /**
   * @brief Saves a newly received or sent message specifically tied to its chat.
   * @param message The message object to append to the chat history.
   */
  virtual void SaveForChat(const CachedMessage& message) = 0;
};

#endif // I_LOCAL_MESSAGE_REPOSITORY_H