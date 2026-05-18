#ifndef I_CLIENT_CHAT_SERVICE_H
#define I_CLIENT_CHAT_SERVICE_H

#include "Chat/chat.h"
#include "Chat/chat_id.h"
#include "Database/local_db_models.h"
#include "Message/message_id.h"

#include <vector>

/**
 * @brief Service responsible for managing chat room entities and their participants.
 * * Implements the cache-aside pattern to fetch chat data from the server
 * and persist it in the local cache repository for fast UI rendering.
 */
class IClientChatService {
public:
  virtual ~IClientChatService() = default;

  /**
   * @brief Retrieves a paginated list of chat rooms the active user belongs to.
   * @param limit The maximum number of chats to fetch.
   * @param offset The pagination offset.
   * @return A vector of CachedChat DTO.
   */
  virtual std::vector<CachedChat> GetChats(std::size_t limit, std::size_t offset) = 0;

  /**
   * @brief Retrieves details for a specific chat room.
   * @param id The unique identifier of the chat room.
   * @return The CachedChat DTO.
   */
  virtual CachedChat GetChatById(const ChatId& id) = 0;

  /**
   * @brief Creates a new chat room on the server and synchronizes it locally.
   * @param participantIds A vector of user IDs to include in the newly created chat.
   */
  virtual void CreateChat(const std::vector<UserId>& participantIds) = 0;

  /**
   * @brief Retrieves the list of users currently participating in a specific chat room.
   * @param chatId The unique identifier of the chat room.
   * @return A vector of CachedUser profiles representing the participants.
   */
  virtual std::vector<CachedUser> GetParticipants(const ChatId& chatId) = 0;
};

#endif // I_CLIENT_CHAT_SERVICE_H