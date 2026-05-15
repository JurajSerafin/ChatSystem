#include "Services/Implementation/notification_service.h"

#include <shared_mutex>
#include <utility>

INotificationService::SubscriberId NotificationService::Subscribe(const UserId& userId, const EventObserver& observer) {
  const SubscriberId id = next_subscriber_id_.fetch_add(1);
  {
    std::unique_lock lock(mutex_);
    subscriptions_[userId].push_back({.id = id, .callback = observer});
  }

  return id;
}

void NotificationService::Unsubscribe(const UserId& userId, SubscriberId subscriberId) {
  {
    std::unique_lock lock(mutex_);

    auto& subs = subscriptions_[userId];

    std::erase_if(subs, [subscriberId](const Subscriber& s) { return s.id == subscriberId; });

    if (subs.empty()) {
      subscriptions_.erase(userId);
    }
  }
}

void NotificationService::Dispatch(const UserId& targetUserId, const NotificationPayloadVariant& payload) {
  std::shared_lock lock(mutex_);

    if (auto it = subscriptions_.find(targetUserId); it != subscriptions_.end()) {

    for (const auto& subscriber : it->second) {
      subscriber.callback(payload);
    }
  }
}

void NotificationService::NotifyNewMessage(const UserId& recipientId, const Message& message) {
  Dispatch(recipientId, NewMessageEvent{.message_id = message.GetId(), .sender_id = message.GetSenderId() });
}

void NotificationService::NotifyReadReceipt(const UserId& senderId, const MessageId& messageId, const UserId& readerId) {
  Dispatch(senderId, ReadReceiptEvent{.message_id = messageId, .reader_id = readerId });
}
