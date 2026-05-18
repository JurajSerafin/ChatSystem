#ifndef NOTIFICATION_EVENTS_H
#define NOTIFICATION_EVENTS_H

#include <Message/message_id.h>
#include <User/user_id.h>
#include <variant>

/**
 * @file notification_events.h
 * @brief Strongly-typed Domain Events for the real-time notification system.
 *
 * These structs represent pure business events that have occurred within the Domain Layer.
 * They are designed to be easily serialized by whatever transport protocol the subscriber uses.
 */

 /**
  * @brief Event emitted when a new E2EE message has been successfully routed and persisted.
  *
  * Triggered by the MessageService. Subscribers use this to push the payload to the
  * recipient's device, prompting it to fetch the new encrypted ciphertext.
  */
struct NewMessageEvent {
  /// @brief The unique identifier of the newly created message.
  MessageId message_id;

  /// @brief The user who sent the message.
  UserId sender_id;
};

/**
 * @brief Event emitted when a user explicitly marks a message as read.
 *
 * Triggered by the IMessageService. Subscribers route this back to the original sender's
 * device so their UI can update the delivery status.
 */
struct ReadReceiptEvent {
  /// @brief The unique identifier of the message that was read.
  MessageId message_id;

  /// @brief The user who triggered the read receipt.
  UserId reader_id;
};

/**
 * @brief A unified payload encompassing all possible real-time domain events.
 *
 * This variant allows the Event Bus (INotificationService) to maintain a single
 * generic observer callback signature, while allowing the Transport Layer to use
 * `std::visit` to safely serialize the underlying specific event.
 * 
 * To add new real-time features in the future, simply define a new struct above
 * and append it to this variant list.
 */
using NotificationPayloadVariant = std::variant<
  NewMessageEvent,
  ReadReceiptEvent
>;

#endif // NOTIFICATION_EVENTS_H