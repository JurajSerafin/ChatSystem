#ifndef NOTIFICATION_SERVICE_H
#define NOTIFICATION_SERVICE_H

#include "Services/i_notification_service.h"
#include <shared_mutex>
#include <unordered_map>
#include <vector>
#include <atomic>

/**
 * @brief Thread-safe, in-memory Publish/Subscribe (PubSub) engine.
 * 
 * Acts as the bridge between the synchronous REST/Service layer and the
 * asynchronous network layer (WebSockets). It tracks active connections
 * and routes real-time events to online users.
 */
class NotificationService : public INotificationService {
public:
  NotificationService() = default;

  /**
   * @brief Registers a callback function for a connected user.
   *
   * Called by the network layer when a WebSocket connection is established.
   *
   * @param userId The user establishing the connection.
   * @param observer The callback function to invoke on new events.
   * @return A unique SubscriberId used to safely unsubscribe later.
   */
  SubscriberId Subscribe(const UserId& userId, const EventObserver& observer) override;

  /**
   * @brief Removes a callback function for a disconnected user.
   *
   * @param userId The user who disconnected.
   * @param subscriberId The unique ID returned during Subscription.
   */
  void Unsubscribe(const UserId& userId, SubscriberId subscriberId) override;

  /**
   * @brief Pushes a new encrypted message event to an online recipient.
   *
   * Silently ignores the event if the recipient is currently offline.
   *
   * @param recipientId The user receiving the message.
   * @param message The encrypted Message object payload.
   */
  void NotifyNewMessage(const UserId& recipientId, const Message& message) override;

  /**
   * @brief Pushes a read receipt event to the original message sender.
   *
   * @param senderId The user who originally sent the message.
   * @param messageId The ID of the message that was read.
   * @param readerId The user who read the message.
   */
  void NotifyReadReceipt(const UserId& senderId, const MessageId& messageId, const UserId& readerId) override;


private:
  struct Subscriber {
    SubscriberId id;
    EventObserver callback;
  };

  mutable std::shared_mutex mutex_;
  std::atomic<SubscriberId> next_subscriber_id_{ 1 };

  using UserSubscribersMap = std::unordered_map<UserId, std::vector<Subscriber>, BaseId<UserId>::Hasher>;

  UserSubscribersMap subscriptions_;

  void Dispatch(const UserId& targetUserId, const NotificationPayloadVariant& payload);
};

#endif // NOTIFICATION_SERVICE_H