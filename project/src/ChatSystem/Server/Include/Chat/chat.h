#ifndef CHAT_H
#define CHAT_H

#include <chrono>
#include <optional>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#include "Id/base_id.h"
#include "chat_id.h"

#include <Message/message.h>
#include <Message/message_id.h>
#include <User/user.h>
#include <User/user_id.h>

class Chat {
private:
    using time_point = std::chrono::system_clock::time_point;

    std::vector<User> participants_;

    std::unordered_map<UserId, User, BaseId<UserId>::Hasher> user_id_map_;
    
    ChatId id_;

    time_point creation_point_;

    std::optional<Message> last_message_;

    mutable std::shared_mutex mutex_;

public:

    Chat(ChatId&& id, time_point created_at) : id_(std::move(id)), creation_point_(created_at) {}

    [[nodiscard]] const std::vector<User>& GetParticipants() const;

    [[nodiscard]] bool HasUser(const UserId& id) const;

    [[nodiscard]] time_point CreatedAt() const;

    [[nodiscard]] bool HasLastMessage() const;

    [[nodiscard]] const std::optional<Message> GetLastMessage() const;

    bool AddParticipant(const User& newParticipant);

    bool RemoveUserById(const UserId& userId);

    void UpdateLastMessage(const Message& message);
    
};

#endif // CHAT_H