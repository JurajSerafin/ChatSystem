#include <Services/Implementation/message_service.h>
#include <Services/Interface/i_notification_service.h>


#include "TypeLibHelpers/domain_class_type_variant_factory.h"
#include <stdexcept>
#include <format>

MessageService::MessageService(
  INotificationService* notificationServiceObs,
  IMessageRepository* messageRepoObs,
  IChatRepository* chatRepoObs
) : notification_service_obs_(notificationServiceObs), message_repo_obs_(messageRepoObs), chat_repo_obs_(chatRepoObs) {
  if (!notification_service_obs_ || !message_repo_obs_ || !chat_repo_obs_) {
    throw std::invalid_argument("MessageService requires valid INotificationService...");
  }
}

void MessageService::EnforceParticipant(const ChatId& chatId, const UserId& userId) const {
  if (!chat_repo_obs_->IsParticipant(chatId, userId)) {
    throw std::logic_error(std::format(
      "User with id {} is not a participant of the chat wit id {}", userId.ToString(), chatId.ToString())
    );
  }
}
void MessageService::EnforceChatExistence(const ChatId& chatId) const {
  if (const auto chat = chat_repo_obs_->FindById(chatId); !chat) {
    throw std::logic_error(std::format("Chat with id {} does not exist.", chatId.ToString()));
  }
}

Message MessageService::SendChatMessage(
  const UserId& senderId,
  const ChatId& chatId,
  const std::string& ciphertext,
  const EncryptedKeysMap& encryptedKeys,
  MessageTypeVariant type) {

  EnforceChatExistence(chatId);

  EnforceParticipant(chatId, senderId);

  MessageParams msg_params{
    .id = MessageId::Generate(),
    .chat_id = chatId,
    .sender_id = senderId,
    .ciphertext = ciphertext,
    .type = type,
    .created_at = std::chrono::system_clock::now(),
  };

  auto new_message = Message::Create(std::move(msg_params), msg_validator_);

  message_repo_obs_->Add(new_message, encryptedKeys);

  chat_repo_obs_->UpdateLastMessage(chatId, new_message);

  auto chat = chat_repo_obs_->FindById(chatId);

  for (auto&& participant_id : chat->GetParticipantIds()) {
    if (participant_id != senderId) {
      notification_service_obs_->NotifyNewMessage(participant_id, new_message);
    }
  }

  return new_message;
}

std::optional<std::string> MessageService::GetEncryptedKey(const MessageId& messageId, const UserId& userId) {
  return message_repo_obs_->GetEncryptedKey(messageId, userId);
}

std::vector<Message> MessageService::GetHistory(const ChatId& chatId, const UserId& requestingUserId, std::size_t limit, std::size_t offset) {
  EnforceChatExistence(chatId);

  EnforceParticipant(chatId, requestingUserId);

  return message_repo_obs_->FindByChatId(chatId, limit, offset);
}

void MessageService::MarkAsRead(const MessageId& id, const UserId& readerId) {
  const auto msg = message_repo_obs_->FindById(id);

  if (!msg) {
    throw std::invalid_argument("Message not found.");
  }

  if (msg->GetSenderId() == readerId) {
    return;
  }

  message_repo_obs_->MarkRead(id, readerId);

  notification_service_obs_->NotifyReadReceipt(msg->GetSenderId(), id, readerId);
}

std::vector<Message> MessageService::GetUndelivered(const UserId& userId, std::size_t limit, std::optional<MessageId> afterMessageId) {
  constexpr std::size_t kMaxLimit = 100;

  const std::size_t safe_limit = std::min(kMaxLimit, limit);

  auto missed_messages = message_repo_obs_->FindUndelivered(userId, safe_limit, afterMessageId);

  for (auto&& msg : missed_messages) {
    message_repo_obs_->MarkDelivered(msg.GetId(), userId);
  }

  return missed_messages;
}

void MessageService::DeleteMessage(const MessageId& id, const UserId& requesterId) {
  const auto msg = message_repo_obs_->FindById(id);

  if (!msg) {
    throw std::invalid_argument("Message not found.");
  }

  if (msg->GetSenderId() != requesterId) {
    throw std::logic_error("You are not allowed delete someone else's message.");
  }

  message_repo_obs_->DeleteById(id);
}
