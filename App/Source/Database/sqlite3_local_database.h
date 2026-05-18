#ifndef SQLITE3_LOCAL_DATABASE_H
#define SQLITE3_LOCAL_DATABASE_H

#include "Database/i_local_database.h"
#include "Infrastructure/Sqlite3/sqlite3_utils.h"
#include "sqlite3.h"

#include <string_view>

/**
 * @brief Concrete implementation of the local database cache using SQLite3.
 * * Executes raw SQL queries, manages the database connection lifecycle,
 * and maps raw SQLite prepared statements directly into C++ domain models.
 */
class Sqlite3LocalDatabase : public ILocalDatabase {
public:
  /** @brief Deleted default constructor to ensure the database path is always provided. */
  Sqlite3LocalDatabase() = delete;

  // Enforce unique ownership of the SQLite connection pointer
  Sqlite3LocalDatabase(const Sqlite3LocalDatabase&) = delete;
  Sqlite3LocalDatabase& operator=(const Sqlite3LocalDatabase&) = delete;

  Sqlite3LocalDatabase(Sqlite3LocalDatabase&& other) noexcept;
  Sqlite3LocalDatabase& operator=(Sqlite3LocalDatabase&& other) noexcept;

  /**
   * @brief Initializes the database connection and ensures the schema exists.
   * @param dbPath The file system path to the SQLite `.db` file.
   */
  explicit Sqlite3LocalDatabase(std::string_view dbPath);

  /** @brief Closes the active SQLite connection and frees memory. */
  ~Sqlite3LocalDatabase() override;


  void SaveIdentity(const CachedIdentity& identity) override;
  std::optional<CachedIdentity> LoadIdentity() override;
  void ClearIdentity() override;

  void UpsertUser(const CachedUser& user) override;
  std::optional<CachedUser> LoadUser(std::string_view userId) override;
  std::optional<CachedUser> LoadUserByLoginOrTag(std::string_view login, std::string_view tag) override;

  void UpsertChat(const CachedChat& chat) override;
  std::optional<CachedChat> LoadChat(std::string_view chatId) override;
  std::vector<CachedChat> LoadChats(std::size_t limit, std::size_t offset) override;

  void AddUserToChat(std::string_view userId, std::string_view chatId, const UserRoleVariant& chatRole) override;
  void SetUserChatRole(std::string_view userId, std::string_view chatId, const UserRoleVariant& chatRole) override;
  std::optional<UserRoleVariant> GetUserChatRole(std::string_view userId, std::string_view chatId) override;
  std::vector<std::string> GetChatParticipantIds(std::string_view chatId) override;

  void SaveMessageForChat(const CachedMessage& message) override;
  std::vector<CachedMessage> LoadMessagesForChat(std::string_view chatId, std::size_t limit, std::size_t offset) override;
  void MarkMessageAsRead(std::string_view messageId) override;
  std::vector<CachedMessage> SearchMessages(std::string_view chatId, std::string_view keyword) override;
  void EvictOldMessages(std::chrono::system_clock::time_point olderThanTimeStamp) override;

  void NukeEntireCache() override;

  void DeleteMessage(std::string_view messageId) override;
  void DeleteUser(std::string_view userId) override;
  void DeleteChat(std::string_view chatId) override;
  void DeleteUserFromChat(std::string_view userId, std::string_view chatId) override;

private:
  /** @brief Raw pointer observing the active SQLite3 database connection. */
  sqlite3* db_obs_;

  /**
   * @brief Executes the Data Definition Language (DDL) queries on startup.
   * * Creates all necessary tables (users, chats, messages, participants) if they do not exist.
   */
  void InitSchema() const;


  /**
   * @brief Extracts a single CachedMessage record from the current row of an evaluated statement.
   * @param rawStmt The executed SQLite statement pointing to a valid row.
   * @param chatId The chat ID to associate with the parsed message.
   * @return The populated CachedMessage struct.
   */
  static CachedMessage ReadMessageFromPreparedStmt(sqlite3_stmt* rawStmt, std::string_view chatId);

  /**
   * @brief Extracts a single CachedChat record from the current row.
   * @param rawStmt The executed SQLite statement pointing to a valid row.
   * @return The populated CachedChat struct.
   */
  static CachedChat ReadChatFromPreparedStmt(sqlite3_stmt* rawStmt);

  /**
   * @brief Extracts a raw string (e.g., an ID) from the first column of the current row.
   * @param rawStmt The executed SQLite statement pointing to a valid row.
   * @return The parsed std::string.
   */
  static std::string ReadStringFromPreparedStmt(sqlite3_stmt* rawStmt);

  /**
   * @brief Extracts the active session identity from the current row.
   * @param rawStmt The executed SQLite statement pointing to a valid row.
   * @return The populated CachedIdentity struct.
   */
  static CachedIdentity ReadIdentityFromPreparedStmt(sqlite3_stmt* rawStmt);

  /**
   * @brief Extracts a CachedUser profile from the current row.
   * @param rawStmt The executed SQLite statement pointing to a valid row.
   * @return The populated CachedUser struct.
   */
  static CachedUser ReadUserFromPreparedStmt(sqlite3_stmt* rawStmt);

  //  Serialization Helpers

  /**
   * @brief Binds the fields of a CachedIdentity to the parameters of a prepared INSERT/UPDATE statement.
   * @param rawStmt The prepared SQLite statement ready for binding.
   * @param identity The identity object supplying the values.
   */
  static void BindIdentity(sqlite3_stmt* rawStmt, const CachedIdentity& identity);

  /**
   * @brief Binds the fields of a CachedUser to a prepared statement.
   * @param rawStmt The prepared SQLite statement ready for binding.
   * @param user The user profile supplying the values.
   */
  static void BindUser(sqlite3_stmt* rawStmt, const CachedUser& user);

  /**
   * @brief Binds the fields of a CachedChat to a prepared statement.
   * @param rawStmt The prepared SQLite statement ready for binding.
   * @param chat The chat metadata supplying the values.
   */
  static void BindChat(sqlite3_stmt* rawStmt, const CachedChat& chat);

  /**
   * @brief Binds the fields of a CachedMessage to a prepared statement.
   * @param rawStmt The prepared SQLite statement ready for binding.
   * @param msg The message object supplying the values.
   */
  static void BindMessage(sqlite3_stmt* rawStmt, const CachedMessage& msg);
};

#endif // SQLITE3_LOCAL_DATABASE_H