#include "Chat/chat.h"
#include "User/user.h"

#include <Chat/chat_validator.h>
#include <Services/Implementation/chat_service.h>
#include <algorithm>
#include <stdexcept>

ChatService::ChatService(IChatRepository* chatRepoObs, IUserRepository* userRepoObs)
  : chat_repo_obs_(chatRepoObs), user_repo_obs_(userRepoObs) {
  if (!chat_repo_obs_ || !user_repo_obs_) {
    throw std::invalid_argument("ChatService requires valid IChatRepository and IUserRepository observers.");
  }
}


void ChatService::EnforceParticipant(const ChatId& chatId, const UserId& userId) const {
  if (!chat_repo_obs_->IsParticipant(chatId, userId)) {
    throw std::logic_error("Access Denied: User is not a participant of this chat.");
  }
}

void ChatService::EnforceUserExistence(const UserId& userId) const {
  if (!user_repo_obs_->FindById(userId)) {
    throw std::invalid_argument("User does not exist.");
  }
}

Chat ChatService::TryFetchChat(const ChatId& chatId) const {
  auto chat = chat_repo_obs_->FindById(chatId);

  if (!chat) {
    throw std::invalid_argument("Chat not found.");
  }

  return std::move(*chat);
}


void ChatService::EnforceUsersExistence(const std::vector<UserId>& userIds) const {
  for (auto&& id : userIds) {
    if (!user_repo_obs_->FindById(id)) {
      throw std::invalid_argument("One or more participants do not exist.");
    }
  }

}

Chat ChatService::CreateNewChatType(const UserId& creatorId, const std::vector<UserId>& participantIds) {
  std::vector<UserId> all_participants{ creatorId };
  all_participants.insert(all_participants.end(), participantIds.begin(), participantIds.end());

  std::sort(all_participants.begin(), all_participants.end());

  all_participants.erase(std::ranges::unique(all_participants).begin(), all_participants.end());
  
  ChatParams params{
      .id = ChatId::Generate(),
      .created_at = std::chrono::system_clock::now(),
      .type = CreateChatType<DirectChatType>(),
      .last_message = std::nullopt,
      .participant_ids = std::move(all_participants)
  };

  const ChatValidator validator;
  Chat new_chat = Chat::Create(std::move(params), validator);

  chat_repo_obs_->Add(new_chat);

  return new_chat;
}

void ChatService::HandleSelfChat(const UserId& selfId, const UserId& otherId) {
  if (selfId == otherId) {
    throw std::invalid_argument("Cannot create a chat with yourself.");
  }
}

Chat ChatService::CreateChat(const UserId& creatorId, const std::vector<UserId>& participantIds) {
  if (participantIds.empty()) {
    throw std::invalid_argument("Cannot create a chat without participants.");
  }

  EnforceUserExistence(creatorId);

  EnforceUsersExistence(participantIds);

  if (participantIds.size() == 1) {
    HandleSelfChat(creatorId, participantIds[0]);

    if (auto existing_chat = FindExistingChat<DirectChatType>(creatorId, participantIds[0])) {
      return std::move(*existing_chat);
    }
  }

  return CreateNewChatType(creatorId, participantIds);
}

std::vector<Chat> ChatService::GetChatsForUser(const UserId& userId, std::size_t limit, std::size_t offset) {
  return chat_repo_obs_->FindByUserId(userId, limit, offset);
}

std::optional<Chat> ChatService::GetById(const ChatId& id, const UserId& requestingUserId) {
  EnforceParticipant(id, requestingUserId);

  return chat_repo_obs_->FindById(id);
}

std::vector<User> ChatService::FetchChatParticipants(const Chat& chat) const {
  std::vector<User> participants;

  for (auto&& user_id : chat.GetParticipantIds()) {
    if (auto user = user_repo_obs_->FindById(user_id)) {
      participants.emplace_back(std::move(*user));
    }
  }

  return participants;
}


std::vector<User> ChatService::GetParticipants(const ChatId& id, const UserId& requestingUserId) {
  const auto chat = TryFetchChat(id);

  EnforceParticipant(id, requestingUserId);

  return FetchChatParticipants(chat);
}

void ChatService::DeleteChat(const ChatId& id, const UserId& requestingUserId) {
  EnforceParticipant(id, requestingUserId);

  chat_repo_obs_->DeleteById(id);
}

void ChatService::AddParticipant(const ChatId& id, const UserId& requestingUserId, const UserId& newUserId) {
  throw std::logic_error("AddParticipant is not yet supported.");
}

void ChatService::RemoveParticipant(const ChatId& id, const UserId& requestingUserId, const UserId& userIdToRemove) {
  throw std::logic_error("RemoveParticipant is not yet supported.");
}
