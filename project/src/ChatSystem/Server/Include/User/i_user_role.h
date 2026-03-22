#ifndef I_USER_ROLE_H
#define I_USER_ROLE_H

#include <string>

#include <User/user_action.h>

class IUserRole {
public:
    [[nodiscard]] virtual const std::string& RoleName() const = 0;
    [[nodiscard]] virtual bool CanPerform(UserAction action) const = 0;
    virtual ~IUserRole() = default;
};

#endif // I_USER_ROLE_H