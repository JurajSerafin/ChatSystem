#ifndef CHAT_SERVICE_H
#define CHAT_SERVICE_H

#include <TypeLibHelpers/domain_class_type_variant_factory.h>

#include <Services/i_chat_service.h>
#include <Repositories/i_chat_repository.h>
#include <Repositories/i_user_repository.h>

/**
 * @brief Manages the lifecycle and access control of conversation threads.
 *
 * Enforces strict business rules to ensure users can only view, modify,
 * or delete chats they are explicitly participating in.
 */
class ChatService : public IChatService {
public:
  explicit ChatService(IChatRepository* chatRepoObs, IUserRepository* userRepoObs);

  /**
   * @brief Initializes a new chat room and links participants.
   *
   * @param creatorId The user initiating the chat.
   * @param participantIds The list of users to include in the chat.
   * @return The newly created Chat domain object.
   * @throws std::invalid_argument if any participant ID does not exist in the database.
   */
  Chat CreateChat(const UserId& creatorId, const std::vector<UserId>& participantIds) override;

  /**
   * @brief Retrieves a paginated list of all chats a user is participating in.
   *
   * @param userId The requesting user.
   * @param limit Maximum number of chats to return.
   * @param offset Pagination offset.
   * @return A vector of Chat objects ordered by recent activity.
   */
  std::vector<Chat> GetChatsForUser(const UserId& userId, std::size_t limit, std::size_t offset) override;

  /**
   * @brief Fetches chat details, enforcing participation access control.
   *
   * @param id The target chat UUID.
   * @param requestingUserId The user requesting the chat data.
   * @return The Chat object, or std::nullopt if it doesn't exist.
   * @throws std::logic_error if the requesting user is not a participant.
   */
  std::optional<Chat> GetById(const ChatId& id, const UserId& requestingUserId) override;

  /**
   * @brief Retrieves the user profiles of everyone in a specific chat.
   *
   * @param id The target chat UUID.
   * @param requestingUserId The user requesting the roster.
   * @return A vector of User objects.
   * @throws std::logic_error if the requesting user is not a participant.
   */
  std::vector<User> GetParticipants(const ChatId& id, const UserId& requestingUserId) override;

  /**
   * @brief Adds a new user to an existing chat.
   *
   * @param id The target chat UUID.
   * @param requestingUserId The user authorizing the addition.
   * @param newUserId The user to be added.
   * @throws std::logic_error if the requesting user lacks permissions or isn't a participant.
   */
  void AddParticipant(const ChatId& id, const UserId& requestingUserId, const UserId& newUserId) override;

  /**
   * @brief Removes a user from a chat.
   *
   * @param id The target chat UUID.
   * @param requestingUserId The user authorizing the removal.
   * @param userIdToRemove The user being removed.
   * @throws std::logic_error if the requesting user lacks permissions or isn't a participant.
   */
  void RemoveParticipant(const ChatId& id, const UserId& requestingUserId, const UserId& userIdToRemove) override;

  /**
   * @brief Completely deletes a chat thread and all associated data.
   *
   * @param id The target chat UUID.
   * @param requestingUserId The user authorizing the deletion.
   * @throws std::logic_error if the requesting user lacks deletion permissions.
   */
  void DeleteChat(const ChatId& id, const UserId& requestingUserId) override;


private:
  IChatRepository* chat_repo_obs_;
  IUserRepository* user_repo_obs_;

  void EnforceParticipant(const ChatId& chatId, const UserId& userId) const;

  void EnforceUserExistence(const UserId& userId) const;

  Chat TryFetchChat(const ChatId& chatId) const;

  void EnforceUsersExistence(const std::vector<UserId>& userIds) const;

  template<ChatType TChatType>
  ChatTypeVariant CreateChatType();

  Chat CreateNewChatType(const UserId& creatorId, const std::vector<UserId>& participantIds);
  
  template<ChatType TChatType>
  std::optional<Chat> FindExistingChat(const UserId& userAId, const UserId& userBId) const;

  std::vector<User> FetchChatParticipants(const Chat& chat) const;

  static void HandleSelfChat(const UserId& selfId, const UserId& otherId);
};


template <ChatType TChatType>
ChatTypeVariant ChatService::CreateChatType() {
  return DomainClassTypeVariantFactory<ChatTypeVariant>::Create(TChatType::TypeString());
}

template <ChatType TChatType>
std::optional<Chat> ChatService::FindExistingChat(const UserId& userAId, const UserId& userBId) const {
  auto user_chats = chat_repo_obs_->FindByUserId(userAId, 100, 0);

  for (auto&& chat : user_chats) {
    if (std::holds_alternative<TChatType>(chat.GetType())) {

      const auto& parts = chat.GetParticipantIds();
      if (std::ranges::find(parts, userBId) != parts.end()) {
        return std::move(chat);
      }
    }
  }
  return std::nullopt;
}

#endif // CHAT_SERVICE_H