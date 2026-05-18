#ifndef I_CLIENT_MESSAGE_SERVICE_H
#define I_CLIENT_MESSAGE_SERVICE_H

#include "Chat/chat_id.h"
#include "Database/local_db_models.h"
#include "Message/message_id.h"

/**
 * @brief Service responsible for the End-to-End Encrypted (E2EE) messaging pipeline.
 * * Handles generating payload keys, encrypting outgoing plaintext, decrypting
 * incoming ciphertext, and persisting plaintext history in the local cache.
 */
class IClientMessageService {
public:
  virtual ~IClientMessageService() = default;

  /**
   * @brief Encrypts and dispatches a text message to a specific chat room.
   * * Generates a symmetric key, encrypts the text, wraps the key for
   * each participant, and posts the payload to the server.
   * @param chatId The destination chat room ID.
   * @param plainText The unencrypted message text.
   */
  virtual void SendMessage(const ChatId& chatId, std::string_view plainText) = 0;

  /**
   * @brief Retrieves a paginated history of fully decrypted messages for a chat.
   * @param chatId The target chat room ID.
   * @param limit The maximum number of messages to fetch.
   * @param offset The pagination offset.
   * @return A vector of decrypted CachedMessage objects.
   */
  virtual std::vector<CachedMessage> GetHistory(const ChatId& chatId, std::size_t limit, std::size_t offset) = 0;

  /**
   * @brief Marks a message as read, updating both the server state and the local cache.
   * @param messageId The unique identifier of the message.
   */
  virtual void MarkAsRead(const MessageId& messageId) = 0;

  /**
   * @brief Fetches, decrypts, and caches the maximum number of messages delivered while the user was offline.
   * @return A vector of newly decrypted CachedMessage objects.
   */
  virtual std::vector<CachedMessage> GetUndelivered() = 0;

  /**
   * @brief Performs a local plaintext search across a chat's history.
   * * Search is executed strictly on the client-side local cache to maintain E2EE security.
   * @param chatId The ID of the chat room to search within.
   * @param keywords The search query string.
   * @return A vector of messages matching the criteria.
   */
  virtual std::vector<CachedMessage> SearchMessages(const ChatId& chatId, std::string_view keywords) = 0;
};

#endif // I_CLIENT_MESSAGE_SERVICE_H