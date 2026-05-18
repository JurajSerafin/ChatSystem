#ifndef I_USER_REPOSITORY_H
#define I_USER_REPOSITORY_H

#include <optional>
#include <string>
#include <vector>

#include <User/user_id.h>
#include <User/user.h>


class IUserRepository {
public:
  virtual ~IUserRepository() = default;

  virtual std::optional<User> FindById(const UserId& id) = 0;

  virtual std::optional<User> FindByLogin(const std::string& login) = 0;

  virtual std::optional<User> FindByTag(const tags::UserTag& tag) = 0;

  virtual std::vector<User> Search(const std::string& query, std::size_t limit, std::size_t offset) = 0;

  virtual void Add(const User& user) = 0;

  virtual void Update(const User& user) = 0;

  virtual void DeleteById(const UserId& id) = 0;
};

#endif // I_USER_REPOSITORY_H


