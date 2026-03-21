#ifndef USER_ID_H
#define USER_ID_H

#include "Id/base_id.h"

class UserId : public BaseId<UserId> {
    friend BaseId;

protected:
    explicit UserId(std::string&& id) : BaseId(std::move(id)) {}
};

#endif // USER_ID_H

