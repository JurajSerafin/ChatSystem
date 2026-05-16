#ifndef I_LOCAL_DATABASE_H
#define I_LOCAL_DATABASE_H

#include "User/Roles/user_role_variant.h"
#include "local_db_models.h"

#include <optional>
#include <vector>
#include <string_view>

class ILocalDatabase {
public:
  virtual ~ILocalDatabase() = default;

  virtual void SaveIdentity(const CachedIdentity& identity) = 0;

  virtual std::optional<CachedIdentity> LoadIdentity() = 0;

  virtual void ClearIdentity() = 0;


  virtual void UpsertUser(const CachedUser& user) = 0;

  virtual std::optional<CachedUser> LoadUser(std::string_view userId) = 0;


  virtual void UpsertChat(const CachedChat& chat) = 0;

  virtual std::optional<CachedChat> LoadChat(std::string_view chatId) = 0;

  virtual std::vector<CachedChat> LoadChats(std::size_t limit, std::size_t offset) = 0;

  virtual void AddUserToChat(std::string_view userId, std::string_view chatId, const UserRoleVariant& chatRole) = 0;

  virtual void SetUserChatRole(std::string_view userId, std::string_view chatId, const UserRoleVariant& chatRole) = 0;

  virtual std::optional<UserRoleVariant> GetUserChatRole(std::string_view userId, std::string_view chatId) = 0;

  virtual std::vector<std::string> GetChatParticipantIds(std::string_view chatId) = 0;

  virtual void DeleteMessage(std::string_view messageId) = 0;

  virtual void DeleteUser(std::string_view userId) = 0;

  virtual void DeleteChat(std::string_view chatId) = 0;

  virtual void DeleteUserFromChat(std::string_view userId, std::string_view chatId) = 0;




  virtual void SaveMessageForChat(const CachedMessage& message) = 0;

  virtual std::vector<CachedMessage> GetMessagesForChat(
    std::string_view chatId,
    std::size_t limit,
    std::size_t offset
  ) = 0;

  virtual void MarkMessageAsRead(std::string_view messageId) = 0;


  virtual void EvictOldMessages(std::chrono::system_clock::time_point olderThanTimeStamp) = 0;

  virtual void NukeEntireCache() = 0;
};

#endif // I_LOCAL_DATABASE_H