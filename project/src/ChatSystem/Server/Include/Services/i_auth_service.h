#ifndef I_AUTH_SERVICE_H
#define I_AUTH_SERVICE_H

#include <string>
#include <optional>

#include <User/user.h>
#include <User/user_id.h>
#include <Session/session.h>


class IAuthService {
public:
    virtual User RegisterUser(std::string const& login, std::string const& password) = 0;

    virtual Session Login(std::string const& login, std::string const& password) = 0;

    virtual void Logout(std::string const& token) = 0;

    virtual std::optional<User> ValidateToken(std::string const& token) = 0;

    virtual void ChangePassword(UserId userId,
                                std::string const& oldPassword,
                                std::string const& newPassword) = 0;
};


#endif  // I_AUTH_SERVICE_H