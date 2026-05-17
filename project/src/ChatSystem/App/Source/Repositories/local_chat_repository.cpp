#include "local_chat_repository.h"

#include <chrono>
#include <limits>
#include <stdexcept>


std::vector<CachedChat> LocalChatRepository::FindAll() {
  return db_obs_->LoadChats(std::numeric_limits<std::size_t>::max(), 0);
}

std::optional<CachedChat> LocalChatRepository::FindById(std::string_view chatId) {
  return db_obs_->LoadChat(chatId);
}

void LocalChatRepository::Upsert(const CachedChat& chat) {
  db_obs_->UpsertChat(chat);
}

void LocalChatRepository::UpdateLastMessage(std::string_view chatId, std::string_view messageId, int64_t lastActivityMs) {
  auto chat_opt = db_obs_->LoadChat(chatId);

  if (!chat_opt.has_value()) {
    throw std::runtime_error("Cannot update last message: Chat not found.");
  }

  chat_opt->last_message_id = std::string{messageId};

  chat_opt->last_activity_at = std::chrono::system_clock::time_point{
    std::chrono::milliseconds{lastActivityMs}
  };

  db_obs_->UpsertChat(*chat_opt);
}

void LocalChatRepository::Delete(std::string_view chatId) {
  db_obs_->DeleteChat(chatId);
}

void LocalChatRepository::AddParticipant(std::string_view userId, std::string_view chatId, const UserRoleVariant& role) {
  db_obs_->AddUserToChat(userId, chatId, role);
}

std::vector<std::string> LocalChatRepository::GetParticipantIds(std::string_view chatId) {
  return db_obs_->GetChatParticipantIds(chatId);
}

void LocalChatRepository::RemoveParticipant(std::string_view userId, std::string_view chatId) {
  db_obs_->DeleteUserFromChat(userId, chatId);
}