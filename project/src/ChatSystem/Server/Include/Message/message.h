#ifndef MESSAGE_H
#define MESSAGE_H

#include <Chat/chat_id.h>
#include <Message/message_id.h>
#include <Message/message_type.h>
#include <User/user_id.h>
#include <chrono>
#include <string>

class Message {
private:
    using time_point = std::chrono::system_clock::time_point;

    std::string content_;

    MessageId id_;

    const ChatId* chat_id_;

    const UserId* sender_id_;

    MessageType type_;

    bool is_read_{false};

    bool is_delivered_{false};

    time_point created_at_;

    Message() = default;

    Message(Message&& other) = default;

    Message(const Message& other) = default;

public:
    class Builder {
    private:
        friend class Message;

        Message message_;

        Builder() = default;

    public:
        [[nodiscard]] Builder& WithId(MessageId&& messageId);

        [[nodiscard]] Builder& WithChatId(const ChatId& chatId);

        [[nodiscard]] Builder& WithSenderId(const UserId& senderId);

        [[nodiscard]] Builder& WithContent(std::string&& content);

        [[nodiscard]] Builder& OfType(const MessageType type);

        [[nodiscard]] Builder& CreatedAt(const time_point createdAt);

        [[nodiscard]] Message Build();
    };

    [[nodiscard]] static Message::Builder CreateBuilder();

    [[nodiscard]] const std::string& GetContent() const;

    [[nodiscard]] time_point CreatedAt() const;

    [[nodiscard]] const MessageId& GetId() const;

    [[nodiscard]] const UserId& GetSenderId() const;

    [[nodiscard]] MessageType GetType() const;

    [[nodiscard]] bool IsRead() const;

    [[nodiscard]] bool IsDelivered() const;

    void SetReadStatus(const bool isRead = true);

    void SetDeliveredStatus(const bool isDelivered = true);
};

// Builder method definitions

inline Message::Builder& Message::Builder::WithId(MessageId&& messageId) {
    message_.id_ = std::move(messageId);
    return *this;
}

inline Message::Builder& Message::Builder::WithChatId(const ChatId& chatId) {
    message_.chat_id_ = &chatId;
    return *this;
}

inline Message::Builder& Message::Builder::WithSenderId(const UserId& senderId) {
    message_.sender_id_ = &senderId;
    return *this;
}

inline Message::Builder& Message::Builder::WithContent(std::string&& content) {
    message_.content_ = std::move(content);
    return *this;
}

inline Message::Builder& Message::Builder::OfType(MessageType type) {
    message_.type_ = type;
    return *this;
}

inline Message::Builder& Message::Builder::CreatedAt(const time_point createdAt) {
    message_.created_at_ = createdAt;
    return *this;
}

inline Message Message::Builder::Build() {
    return Message{std::move(message_)};
}

inline Message::Builder Message::CreateBuilder() {
    return Message::Builder{};
}

// Message method definitions

inline const std::string& Message::GetContent() const {
    return content_;
}

inline Message::time_point Message::CreatedAt() const {
    return created_at_;
}

inline const MessageId& Message::GetId() const {
    return id_;
}

inline const UserId& Message::GetSenderId() const {
    return *sender_id_;
}

inline MessageType Message::GetType() const {
    return type_;
}

inline bool Message::IsRead() const {
    return is_read_;
}

inline bool Message::IsDelivered() const {
    return is_delivered_;
}

inline void Message::SetReadStatus(const bool isRead) {
    is_read_ = isRead;
}

inline void Message::SetDeliveredStatus(const bool isDelivered) {
    is_delivered_ = isDelivered;
}

#endif