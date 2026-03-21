#ifndef CHAT_ID_H
#define CHAT_ID_H

#include <Id/base_id.h>

class ChatId : public BaseId<ChatId>{
    friend BaseId;

protected:
    explicit ChatId(std::string&& id) : BaseId(std::move(id)) {}
};

#endif // CHAT_ID_H