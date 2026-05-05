#ifndef I_USER_REPOSITORY_H
#define I_USER_REPOSITORY_H

#include <optional>
#include <string>
#include <vector>

#include <User/user_id.h>
#include <User/user.h>


class IUserRepository {
public:

    virtual std::optional<User> FindById(UserId id) = 0;
    virtual std::optional<User> FindByLogin(const std::string& login) = 0;
    virtual std::optional<User> FindByTag(const std::string& tag) = 0;

    virtual std::vector<User> Search(const std::string& query) = 0;

    virtual User Create(const User& user) = 0;
    virtual User Create(User&& user) = 0;

    virtual void Update(const User& user) = 0;
    virtual void DeleteById(UserId id) = 0;

    virtual void StorePublicKey(UserId id, const std::string& publicKey) = 0;
    virtual std::optional<std::string> GetPublicKey(UserId id) = 0;
};

#endif // I_USER_REPOSITORY_H


