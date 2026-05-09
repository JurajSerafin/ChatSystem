#ifndef I_SESSION_REPOSITORY_H
#define I_SESSION_REPOSITORY_H

#include <optional>
#include <vector>
#include <string>

#include <Session/session.h>
#include <User/user_id.h>


class ISessionRepository {
public:
  virtual ~ISessionRepository() = default;

  virtual Session Create(const Session& session) = 0;

  virtual std::optional<Session> FindByToken(const std::string& token) = 0;

  virtual std::vector<Session> FindByUserId(const UserId& userId) = 0;

  virtual void DeleteByToken(const std::string& token) = 0;

  virtual void DeleteAllForUser(const UserId& userId) = 0;

  virtual void DeleteExpired() = 0;
};

#endif // I_SESSION_REPOSITORY_H

