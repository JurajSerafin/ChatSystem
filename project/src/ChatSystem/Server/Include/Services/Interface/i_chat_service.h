#ifndef I_CHAT_SERVICE_H
#define I_CHAT_SERVICE_H

#include <optional>
#include <vector>

#include <Chat/chat.h>
#include <Chat/chat_id.h>
#include <User/User_id.h>
#include <User/user.h>

/**
 * @brief Interface for chat lifecycle and participant management.
 *
 * Orchestrates the creation of chat rooms,
 * enforces membership authorization before allowing read/write operations,
 * and handles chat list retrieval.
 */
class IChatService {
public:
  virtual ~IChatService() = default;

  /**
   * @brief Creates a new chat or returns an existing one.
   *
   * For 1-on-1 chats, this method must enforce deduplication: if a direct chat
   * already exists between the creator and the participant, the existing chat
   * is returned.
   *
   * @param creatorId The ID of the user creating the chat.
   * @param participantIds A list of IDs to include in the chat.
   * @return The newly created or existing Chat domain object.
   * @throws std::invalid_argument if participants do not exist or if attempting self-chat.
   */
  virtual Chat CreateChat(const UserId& creatorId, const std::vector<UserId>& participantIds) = 0;

  /**
   * @brief Retrieves a paginated list of chats for a specific user.
   *
   * Results should be ordered by the most recent message timestamp.
   *
   * @param userId The ID of the user requesting their chats.
   * @param limit The maximum number of chats to return.
   * @param offset The pagination offset.
   * @return A vector of Chat objects containing the latest message previews.
   */
  virtual std::vector<Chat> GetChatsForUser(const UserId& userId, std::size_t limit, std::size_t offset) = 0;

  /**
   * @brief Retrieves a specific chat by its ID.
   *
   * @param id The target chat ID.
   * @param requestingUserId The user requesting the chat. Must be a participant.
   * @return The Chat object if found, or std::nullopt.
   * @throws std::logic_error if the requesting user is not a participant.
   */
  virtual std::optional<Chat> GetById(const ChatId& id, const UserId& requestingUserId) = 0;

  /**
   * @brief Retrieves the user profiles of all participants in a chat.
   *
   * @param id The target chat ID.
   * @param requestingUserId The user requesting the list. Must be a participant.
   * @return A vector of User objects representing the participants.
   * @throws std::logic_error if the requesting user is not a participant.
   */
  virtual std::vector<User> GetParticipants(const ChatId& id, const UserId& requestingUserId) = 0;

  /**
   * @brief Adds a new participant to an existing group chat.
   *
   * @param id The target chat ID.
   * @param requestingUserId The user performing the action. Must have permissions.
   * @param newUserId The ID of the user being added.
   * @throws std::logic_error if the chat is an immutable Direct Message.
   */
  virtual void AddParticipant(const ChatId& id, const UserId& requestingUserId, const UserId& newUserId) = 0;

  /**
   * @brief Removes a participant from an existing group chat.
   *
   * @param id The target chat ID.
   * @param requestingUserId The user performing the action.
   * @param userIdToRemove The ID of the user being removed.
   * @throws std::logic_error if the chat is an immutable Direct Message.
   */
  virtual void RemoveParticipant(const ChatId& id, const UserId& requestingUserId, const UserId& userIdToRemove) = 0;

  /**
   * @brief Deletes a chat entirely.
   *
   * @param id The target chat ID.
   * @param requestingUserId The user requesting deletion. Must be a participant.
   */
  virtual void DeleteChat(const ChatId& id, const UserId& requestingUserId) = 0;
};

#endif // I_CHAT_SERVICE_H