#ifndef I_NOTIFICATION_SERVICE_H
#define I_NOTIFICATION_SERVICE_H

#include <User/user_id.h>
#include <Message/message_id.h>
#include <Message/message.h>

/**
 * @brief Interface for real-time signaling and push notifications.
 *
 * Typically implemented by a WebSocket controller or a push notification,
 * to push state changes to connected clients.
 */
class INotificationService {
public:
  virtual ~INotificationService() = default;

  /**
   * @brief Notifies an online user that they have received a new message.
   *
   * @param recipientId The target user to notify.
   * @param message The newly arrived message object.
   */
  virtual void NotifyNewMessage(const UserId& recipientId, const Message& message) = 0;

  /**
   * @brief Pushes a read receipt back to the original sender of a message.
   *
   * @param senderId The user who originally sent the message.
   * @param messageId The message that was just read.
   * @param readerId The user who triggered the read receipt.
   */
  virtual void NotifyReadReceipt(const UserId& senderId, MessageId messageId, UserId readerId) = 0;

  /**
   * @brief Broadcasts to relevant contacts that a user has come online.
   * @param userId The user who established a WebSocket connection.
   */
  virtual void NotifyUserOnline(const UserId& userId) = 0;

  /**
   * @brief Broadcasts to relevant contacts that a user has gone offline.
   * @param userId The user whose WebSocket connection dropped.
   */
  virtual void NotifyUserOffline(const UserId& userId) = 0;
};

#endif // I_NOTIFICATION_SERVICE_H