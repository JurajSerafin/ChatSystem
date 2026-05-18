#ifndef I_NOTIFICATION_SERVICE_H
#define I_NOTIFICATION_SERVICE_H

#include "Services/notification_events.h"
#include <User/user_id.h>
#include <Message/message_id.h>
#include <Message/message.h>
#include <functional>

/**
 * @brief Protocol-agnostic Event Bus for real-time domain events.
 *
 * This interface acts as a Event bus or Pub/Sub message broker, bridging the gap between 
 * the Domain Layer and the Transport Layer, without coupling them together.
 *
 * - The Transport Layer acts as a Publisher, registering callbacks here 
 * to listen for events for specific users.
 * - The Domain Layer (e.g., MessageService) acts as a Subscriber, pushing
 * strongly-typed C++ events here without needing to know who is listening
 * or how the events are delivered.
 */
class INotificationService {
public:
  virtual ~INotificationService() = default;

  /**
   * @brief A unique identifier representing a specific subscriber connection.
   * Allows a single user to have multiple concurrent devices (e.g., a phone and a desktop)
   * subscribed simultaneously.
   */
  using SubscriberId = uint64_t;

  /**
   * @brief The callback executed when a domain event occurs for the subscribed user.
   * The transport layer implements this to serialize the NotificationPayloadVariant
   * and push it across the network.
   */
  using EventObserver = std::function<void(const NotificationPayloadVariant& event)>;

  /**
   * @brief Registers an observer to listen for real-time events directed at a user.
   *
   * @param userId The user whose events the observer wants to listen to.
   * @param observer The callback function to execute when an event occurs.
   * @return A unique SubscriberId used to deregister this specific connection later.
   */
  virtual SubscriberId Subscribe(const UserId& userId, const EventObserver& observer) = 0;

  /**
   * @brief Deregisters a specific observer connection.
   *
   * Typically called when a WebSocket drops, a user logs out, or an APNs token expires.
   *
   * @param userId The user who owns the subscription.
   * @param subscriberId The unique connection token returned by Subscribe().
   */
  virtual void Unsubscribe(const UserId& userId, SubscriberId subscriberId) = 0;


  /**
   * @brief Pushes a new message notification to the recipient's active devices.
   *
   * Called internally by the backend after a message has been successfully
   * routed and atomically persisted to the database.
   *
   * @param recipientId The target user to notify.
   * @param message The newly arrived message domain object.
   */
  virtual void NotifyNewMessage(const UserId& recipientId, const Message& message) = 0;

  /**
   * @brief Pushes a read receipt back to the original sender's active devices.
   *
   * Called internally when a user explicitly marks a message as read in their client.
   *
   * @param senderId The user who originally sent the message (the target of this notification).
   * @param messageId The ID of the message that was just read.
   * @param readerId The user who triggered the read receipt.
   */
  virtual void NotifyReadReceipt(const UserId& senderId, const MessageId& messageId, const UserId& readerId) = 0;
};

#endif // I_NOTIFICATION_SERVICE_H