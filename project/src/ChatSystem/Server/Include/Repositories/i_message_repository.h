#ifndef I_MESSAGE_REPOSITORY_H
#define I_MESSAGE_REPOSITORY_H

#include "Message/encrypted_keys_map.h"
#include "Message/message.h"

#include <Message/message_id.h>
#include <User/user_id.h>
#include <vector>

/**
 * @brief Server-side repository interface for managing End-to-End Encrypted (E2EE) messages.
 * * Responsible for persisting message ciphertexts, their corresponding wrapped symmetric keys,
 * and tracking delivery/read receipts across participants.
 */
class IMessageRepository {
public:
  virtual ~IMessageRepository() = default;

  /**
   * @brief Retrieves a specific message by its unique identifier.
   * @param id The ID of the message.
   * @return An optional containing the message if found.
   */
  [[nodiscard]] virtual std::optional<Message> FindById(const MessageId& id) = 0;

  /**
   * @brief Retrieves the paginated message history for a specific chat room.
   * @param chatId The ID of the target chat room.
   * @param limit The maximum number of messages to return.
   * @param offset The pagination offset.
   * @return A vector of messages.
   */
  [[nodiscard]] virtual std::vector<Message> FindByChatId(const ChatId& chatId, std::size_t limit, std::size_t offset) = 0;

  /**
   * @brief Retrieves a paginated list of pending messages that the recipient has not yet downloaded.
   * @param recipientId The ID of the offline user.
   * @param limit The maximum number of undelivered messages to return.
   * @param afterMessageId Optional cursor ID to fetch messages strictly newer than a specific point.
   * @return A vector of pending messages.
   */
  [[nodiscard]] virtual std::vector<Message> FindUndelivered(const UserId& recipientId, std::size_t limit, std::optional<MessageId> afterMessageId) = 0;

  /**
   * @brief Retrieves the list of user IDs who have explicitly marked this message as read.
   * @param messageId The target message ID.
   * @return A vector of user IDs.
   */
  [[nodiscard]] virtual std::vector<UserId> GetReaders(const MessageId& messageId) = 0;

  /**
   * @brief Retrieves the list of user IDs to whom this message has been successfully delivered.
   * @param messageId The target message ID.
   * @return A vector of user IDs.
   */
  [[nodiscard]] virtual std::vector<UserId> GetDeliveredTo(const MessageId& messageId) = 0;

  /**
   * @brief Permanently deletes a message from the server database.
   * @param id The ID of the message to delete.
   */
  virtual void DeleteById(const MessageId& id) = 0;

  /**
   * @brief Records that a specific user has successfully downloaded/received a message.
   * @param messageId The target message ID.
   * @param recipientId The ID of the user who received it.
   */
  virtual void MarkDelivered(const MessageId& messageId, const UserId& recipientId) = 0;

  /**
   * @brief Records that a specific user has explicitly read a message.
   * @param messageId The target message ID.
   * @param readerId The ID of the user who read it.
   */
  virtual void MarkRead(const MessageId& messageId, const UserId& readerId) = 0;

  /**
   * @brief Adds a new message payload along with its E2EE wrapped key dictionary.
   * @param message The core message metadata and ciphertext.
   * @param encryptedKeys A map linking every participant's UserId to their uniquely wrapped symmetric key.
   */
  virtual void Add(const Message& message, const EncryptedKeysMap& encryptedKeys) = 0;

  /**
   * @brief Retrieves the specific wrapped symmetric key meant for a specific user to unlock a message.
   * @param messageId The target message ID.
   * @param userId The ID of the requesting user.
   * @return An optional containing the base64-encoded wrapped key string.
   */
  virtual std::optional<std::string> GetEncryptedKey(const MessageId& messageId, const UserId& userId) = 0;
};

#endif // I_MESSAGE_REPOSITORY_H