#ifndef MESSAGE_SERVICE_H
#define MESSAGE_SERVICE_H

#include "Message/encrypted_keys_map.h"
#include "Message/message_validator.h"
#include "Services/Interface/i_message_service.h"
#include "Services/Interface/i_notification_service.h"

#include <Repositories/i_chat_repository.h>
#include <Repositories/i_message_repository.h>


/**
 * @brief Core orchestrator for the End-to-End Encrypted (E2EE) messaging flow.
 *
 * This service coordinates the atomic storage of encrypted message payloads
 * alongside user-specific wrapped AES session keys. It also manages message
 * lifecycle states (delivered, read), enforces participant-only access control,
 * and interfaces with the NotificationService to trigger real-time WebSocket events.
 */
class MessageService : public IMessageService {
public:
  explicit MessageService(
    INotificationService* notificationServiceObs,
    IMessageRepository* messageRepoObs,
    IChatRepository* chatRepoObs
  );

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
  Message SendChatMessage(
    const UserId& senderId,
    const ChatId& chatId,
    const std::string& ciphertext,
    const EncryptedKeysMap& encryptedKeys,
    MessageTypeVariant type) override;

  /**
   * @brief Retrieves the specific encrypted AES key for a user to decrypt a message.
   *
   * @param messageId The ID of the target message.
   * @param userId The ID of the user requesting their specific wrapped key.
   * @return The Base64 string of the wrapped key, or std::nullopt if not found.
   */
  std::optional<std::string> GetEncryptedKey(const MessageId& messageId, const UserId& userId) override;

  /**
   * @brief Retrieves a paginated history of messages for a chat.
   *
   * @param chatId The target chat.
   * @param requestingUserId The user requesting the history. Must be a participant.
   * @param limit The maximum number of messages to return.
   * @param offset The pagination offset.
   * @return A vector of Message objects (containing ciphertext).
   */
  std::vector<Message> GetHistory(const ChatId& chatId, const UserId& requestingUserId, std::size_t limit, std::size_t offset) override;

  /**
   * @brief Marks a message as read and notifies the original sender.
   *
   * @param id The message being marked as read.
   * @param readerId The user who read the message.
   */
  void MarkAsRead(const MessageId& id, const UserId& readerId) override;

  /**
   * @brief Fetches all messages that arrived while a user was offline.
   *
   * This method automatically marks the fetched messages as "Delivered" upon retrieval.
   *
   * @param userId The reconnecting user.
   * @return A vector of undelivered messages.
   */
  std::vector<Message> GetUndelivered(const UserId& userId, std::size_t limit, std::optional<MessageId> afterMessageId) override;

  /**
   * @brief Deletes a specific message.
   *
   * @param id The message to delete.
   * @param requesterId The user requesting deletion. Must be the original sender.
   * @throws std::logic_error if the requester is not the sender.
   */
  void DeleteMessage(const MessageId& id, const UserId& requesterId) override;

private:
  void EnforceParticipant(const ChatId& chatId, const UserId& userId) const;

  void EnforceChatExistence(const ChatId& chatId) const;

  MessageValidator msg_validator_;

  INotificationService* notification_service_obs_;

  IMessageRepository* message_repo_obs_;
  IChatRepository* chat_repo_obs_;
};


#endif // MESSAGE_SERVICE_H