#ifndef I_USER_ROLE_H
#define I_USER_ROLE_H

#include <string>

#include <Actions/i_action.h>

class IUserRole {
public:
    virtual std::string RoleName() const = 0;
    virtual bool CanPerform(IAction action) const = 0;
    virtual ~IUserRole() = default;
};

#endif // I_USER_ROLE_H