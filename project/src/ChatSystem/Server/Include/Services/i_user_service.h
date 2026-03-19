#ifndef I_USER_SERVICE_H
#define I_USER_SERVICE_H

#include <vector>
#include <string>
#include <optional>

#include <User/user.h>
#include <User/user_id.h>

struct IUserService {
    // Search bar — searches by login or tag
    virtual std::vector<User> Search(std::string const& query) = 0;

    virtual std::optional<User> GetById(UserId id) = 0;
    virtual std::optional<User> GetByTag(std::string const& tag) = 0;

    // Fetching a user's public key before encrypting a message to them
    virtual std::optional<std::string> GetPublicKey(UserId userId) = 0;
};

#endif // I_USER_SERVICE_H