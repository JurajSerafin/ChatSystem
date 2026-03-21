#ifndef I_AUTH_SERVICE_H
#define I_AUTH_SERVICE_H

#include <string>
#include <optional>

#include <User/user.h>
#include <User/user_id.h>
#include <Session/session.h>


class IAuthService {
public:
    virtual User RegisterUser(const std::string& login, const std::string& password) = 0;

    virtual Session Login(const std::string& login, const std::string& password) = 0;

    virtual void Logout(const std::string& token) = 0;

    virtual std::optional<User> ValidateToken(const std::string& token) = 0;

    virtual void ChangePassword(UserId userId, const std::string& oldPassword, const std::string& newPassword) = 0;
};


#endif  // I_AUTH_SERVICE_H