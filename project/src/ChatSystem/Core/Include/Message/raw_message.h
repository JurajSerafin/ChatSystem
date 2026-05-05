#ifndef RAW_MESSAGE_H
#define RAW_MESSAGE_H

#include <chrono>
#include <string>

#include <Message/message_type.h>
#include <User/user_id.h>
#include <Chat/chat_id.h>


struct RawMessage {
    UserId sender_id;

    ChatId chat_id;

    std::string content;

    MessageType type;

    std::chrono::system_clock::time_point timestamp;
};


#endif // RAW_MESSAGE_H