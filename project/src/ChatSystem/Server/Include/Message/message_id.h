#ifndef MESSAGE_ID
#define MESSAGE_ID

#include "Id/base_id.h"

class MessageId : public BaseId<MessageId> {
    friend BaseId;

protected:
    explicit MessageId(std::string&& id) : BaseId(std::move(id)) {}
};

#endif // MESSAGE_H