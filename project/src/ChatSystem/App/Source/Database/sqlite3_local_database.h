#ifndef SQLITE3_LOCAL_DATABASE_H
#define SQLITE3_LOCAL_DATABASE_H

#include "Database/i_local_database.h"
#include "sqlite3.h"
#include <string_view>

class Sqlite3LocalDatabase : public ILocalDatabase {
public:
  Sqlite3LocalDatabase() = delete;

  Sqlite3LocalDatabase(const Sqlite3LocalDatabase&) = delete;
  Sqlite3LocalDatabase& operator=(const Sqlite3LocalDatabase&) = delete;

  Sqlite3LocalDatabase(Sqlite3LocalDatabase&& other) noexcept;
  Sqlite3LocalDatabase& operator=(Sqlite3LocalDatabase&& other) noexcept;

  explicit Sqlite3LocalDatabase(std::string_view dbPath);

  ~Sqlite3LocalDatabase() override;


  void SaveIdentity(const CachedIdentity& identity) override;

  std::optional<CachedIdentity> LoadIdentity() override;

  void ClearIdentity() override;


  void UpsertUser(const CachedUser& user) override;

  std::optional<CachedUser> LoadUser(std::string_view userId) override;


  void UpsertChat(const CachedChat& chat) override;

  std::optional<CachedChat> LoadChat(std::string_view chatId) override;

  std::vector<CachedChat> LoadChats(std::size_t limit, std::size_t offset) override;

  void AddUserToChat(std::string_view userId, std::string_view chatId, const UserRoleVariant& chatRole) override;

  void SetUserChatRole(std::string_view userId, std::string_view chatId, const UserRoleVariant& chatRole) override;

  std::optional<UserRoleVariant> GetUserChatRole(std::string_view userId, std::string_view chatId) override;

  std::vector<std::string> GetChatParticipantIds(std::string_view chatId) override;

  void SaveMessageForChat(const CachedMessage& message) override;

  std::vector<CachedMessage> GetMessagesForChat(
    std::string_view chatId,
    std::size_t limit,
    std::size_t offset
  ) override;

  void MarkMessageAsRead(std::string_view messageId) override;

  void EvictOldMessages(std::chrono::system_clock::time_point olderThanTimeStamp) override;

  void NukeEntireCache() override;

  void DeleteMessage(std::string_view messageId) override;

  void DeleteUser(std::string_view userId) override;

  void DeleteChat(std::string_view chatId) override;

  void DeleteUserFromChat(std::string_view userId, std::string_view chatId) override;


private:
  sqlite3* db_obs_;

  void InitSchema() const;

  static CachedMessage ReadMessageFromPreparedStmt(sqlite3_stmt* rawStmt, std::string_view chatId);

  static CachedChat ReadChatFromPreparedStmt(sqlite3_stmt* rawStmt);

  static std::string ReadStringFromPreparedStmt(sqlite3_stmt* rawStmt);

  static CachedIdentity ReadIdentityFromPreparedStmt(sqlite3_stmt* rawStmt);

  static void BindIdentity(sqlite3_stmt* rawStmt, const CachedIdentity& identity);

  static void BindUser(sqlite3_stmt* rawStmt, const CachedUser& user);

  static void BindChat(sqlite3_stmt* rawStmt, const CachedChat& chat);

  static void BindMessage(sqlite3_stmt* rawStmt, const CachedMessage& msg);

};


#endif // SQLITE3_LOCAL_DATABASE_H