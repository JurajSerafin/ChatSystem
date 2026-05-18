#include "local_message_repository.h"

#include <execution>

std::vector<CachedMessage> LocalMessageRepository::FindByChatId(std::string_view chatId, std::size_t limit, std::size_t offset) {
  return db_obs_->LoadMessagesForChat(chatId, limit, offset);
}

std::vector<CachedMessage> LocalMessageRepository::Search(std::string_view chatId, std::string_view keywords) {
  return db_obs_->SearchMessages(chatId, keywords);
}

void LocalMessageRepository::Delete(std::string_view messageId) {
  db_obs_->DeleteMessage(messageId);
}

void LocalMessageRepository::MarkAsRead(std::string_view messageId) {
  db_obs_->MarkMessageAsRead(messageId);
}

void LocalMessageRepository::SaveForChat(const CachedMessage& message) {
  db_obs_->SaveMessageForChat(message);
}
