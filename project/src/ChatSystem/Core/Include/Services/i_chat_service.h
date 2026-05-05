#ifndef I_CHAT_SERVICE_H
#define I_CHAT_SERVICE_H

#include <optional>
#include <vector>

#include <Chat/chat.h>
#include <Chat/chat_id.h>
#include <User/User_id.h>
#include <User/user.h>


class IChatService {
public:
    virtual Chat CreateChat(UserId creatorId, const std::vector<UserId>& participantIds) = 0;

    virtual std::vector<Chat> GetChatsForUser(UserId userId) = 0;

    virtual std::optional<Chat> GetChatById(ChatId id) = 0;
    virtual std::vector<User> GetParticipants(ChatId id) = 0;

    virtual void AddParticipant(ChatId chatId, UserId userId) = 0;
    virtual void RemoveParticipant(ChatId chatId, UserId userId) = 0;
    virtual void DeleteChat(ChatId chatId, UserId requesterId) = 0;
};

#endif // I_CHAT_SERVICE_H