#ifndef I_MESSAGE_SERVICE_H
#define I_MESSAGE_SERVICE_H

#include "Message/encrypted_keys_map.h"

#include <Chat/chat_id.h>
#include <Message/message.h>
#include <Message/message_id.h>
#include <User/user_id.h>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Interface for Zero-Knowledge message routing and synchronization.
 *
 * This service operates purely on ciphertext. It routes End-to-End Encrypted (E2EE)
 * payloads between clients, tracks delivery/read receipts, and synchronizes
 * offline message histories.
 */
class IMessageService {
public:
  virtual ~IMessageService() = default;

  /**
   * @brief Persists and routes an E2EE message.
   *
   * Validates the sender, atomically persists the ciphertext and the wrapped AES keys
   * to the database, updates the chat's last message timestamp, and triggers real-time
   * push notifications for online recipients.
   *
   * @param senderId The user sending the message.
   * @param chatId The destination chat.
   * @param ciphertext The message content symmetrically encrypted with an AES key.
   * @param encryptedKeys A map linking each participant's UserId to the AES key, wrapped with their public RSA key.
   * @param type The message type indicator
   * @return The Message domain object.
   * @throws std::logic_error if the sender is not a participant in the chat.
   */
  virtual Message SendChatMessage(
    const UserId& senderId,
    const ChatId& chatId,
    const std::string& ciphertext,
    const EncryptedKeysMap& encryptedKeys,
    MessageTypeVariant type) = 0;

  /**
   * @brief Retrieves the specific encrypted AES key for a user to decrypt a message.
   *
   * @param messageId The ID of the target message.
   * @param userId The ID of the user requesting their specific wrapped key.
   * @return The Base64 string of the wrapped key, or std::nullopt if not found.
   */
  virtual std::optional<std::string> GetEncryptedKey(const MessageId& messageId, const UserId& userId) = 0;

  /**
   * @brief Retrieves a paginated history of messages for a chat.
   *
   * @param chatId The target chat.
   * @param requestingUserId The user requesting the history. Must be a participant.
   * @param limit The maximum number of messages to return.
   * @param offset The pagination offset.
   * @return A vector of Message objects (containing ciphertext).
   */
  virtual std::vector<Message> GetHistory(const ChatId& chatId, const UserId& requestingUserId, std::size_t limit, std::size_t offset) = 0;

  /**
   * @brief Marks a message as read and notifies the original sender.
   *
   * @param id The message being marked as read.
   * @param readerId The user who read the message.
   */
  virtual void MarkAsRead(const MessageId& id, const UserId& readerId) = 0;

  /**
   * @brief Fetches all messages that arrived while a user was offline.
   *
   * This method automatically marks the fetched messages as "Delivered" upon retrieval.
   *
   * @param userId The reconnecting user.
   * @return A vector of undelivered messages.
   */
  virtual std::vector<Message> GetUndelivered(const UserId& userId, std::size_t limit, std::optional<MessageId> afterMessageId) = 0;

  /**
   * @brief Deletes a specific message.
   *
   * @param id The message to delete.
   * @param requesterId The user requesting deletion. Must be the original sender.
   * @throws std::logic_error if the requester is not the sender.
   */
  virtual void DeleteMessage(const MessageId& id, const UserId& requesterId) = 0;
};

#endif // I_MESSAGE_SERVICE_H