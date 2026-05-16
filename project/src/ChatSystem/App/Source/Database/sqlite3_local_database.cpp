#include "sqlite3_local_database.h"

#include "Infrastructure/Sqlite3/sqlite3_utils.h"

#include <utility>

Sqlite3LocalDatabase::Sqlite3LocalDatabase(Sqlite3LocalDatabase&& other) noexcept
  : db_obs_(std::exchange(other.db_obs_, nullptr)) {}

Sqlite3LocalDatabase& Sqlite3LocalDatabase::operator=(Sqlite3LocalDatabase&& other) noexcept {
  if (this != &other) {

    if (db_obs_) {
      sqlite3_close(db_obs_);
    }

    db_obs_ = std::exchange(other.db_obs_, nullptr);
  }

  return *this;
}

Sqlite3LocalDatabase::Sqlite3LocalDatabase(std::string_view dbPath) {
  if (sqlite3_open(dbPath.data(), &db_obs_) != SQLITE_OK) {
    const std::string error = sqlite3_errmsg(db_obs_);

    sqlite3_close(db_obs_);

    throw std::runtime_error("Failed to open SQLite database: " + error);
  }
  InitSchema();
}

Sqlite3LocalDatabase::~Sqlite3LocalDatabase() {
  if (db_obs_) {
    sqlite3_close(db_obs_);
  }
}

void Sqlite3LocalDatabase::InitSchema() const {
  constexpr auto sql = R"(
    CREATE TABLE IF NOT EXISTS identity (
      id TEXT PRIMARY KEY,
      login TEXT NOT NULL,
      tag TEXT NOT NULL,
      session_token TEXT
    );
    CREATE TABLE IF NOT EXISTS cached_users (
      id TEXT PRIMARY KEY,
      login TEXT NOT NULL,
      tag TEXT NOT NULL,
      public_key TEXT NOT NULL,
      cached_at INTEGER NOT NULL,
      is_deleted INTEGER NOT NULL DEFAULT 0
    );
    CREATE TABLE IF NOT EXISTS cached_chats (
      id TEXT PRIMARY KEY,
      name TEXT, last_message_id TEXT,
      last_activity_at INTEGER NOT NULL,
      cached_at INTEGER NOT NULL,
      is_deleted INTEGER NOT NULL DEFAULT 0
    );
    CREATE TABLE IF NOT EXISTS cached_chat_participants (
      chat_id TEXT NOT NULL,
      user_id TEXT NOT NULL,
      role TEXT NOT NULL,
      is_deleted INTEGER NOT NULL DEFAULT 0,
      PRIMARY KEY (chat_id, user_id)
    );
    CREATE TABLE IF NOT EXISTS cached_messages (
      id TEXT PRIMARY KEY,
      chat_id TEXT NOT NULL,
      sender_id TEXT NOT NULL,
      plaintext TEXT NOT NULL, 
      type TEXT NOT NULL,
      created_at INTEGER NOT NULL,
      is_read INTEGER NOT NULL DEFAULT 0,
      is_delivered INTEGER NOT NULL DEFAULT 0,
      is_deleted INTEGER NOT NULL DEFAULT 0
    );
    CREATE INDEX IF NOT EXISTS idx_cached_messages_chat ON cached_messages(chat_id, created_at DESC);
  )";

  Sqlite3Utils::InitSchemaOrThrow(db_obs_, sql);
}

void Sqlite3LocalDatabase::UpsertUser(const CachedUser& user) {
  constexpr auto sql = R"(
    INSERT OR REPLACE INTO cached_users (id, login, tag, public_key, cached_at, is_deleted) 
    VALUES (?, ?, ?, ?, ?, ?);
  )";

  sqlite3_stmt* raw_stmt = nullptr;

  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Prepare failed: UpsertUser");
  }

  const Sqlite3Utils::ScopedStmt stmt{ raw_stmt };

  BindUser(stmt.get(), user);

  if (!Sqlite3Utils::TryExecuteStore(stmt.get())) {
    throw std::runtime_error("Execute failed: UpsertUser");
  }
}

std::optional<CachedUser> Sqlite3LocalDatabase::LoadUser(std::string_view userId) {
  constexpr auto sql = "SELECT login, tag, public_key, cached_at, is_deleted FROM cached_users WHERE id = ?;";

  sqlite3_stmt* raw_stmt = nullptr;

  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Prepare failed: LoadUser");
  }

  const Sqlite3Utils::ScopedStmt stmt{ raw_stmt };

  Sqlite3Utils::BindStr(stmt.get(), userId, 1);

  if (!Sqlite3Utils::TryExecuteLoad(stmt.get())) {
    return std::nullopt;
  }

  CachedUser user;

  user.id = std::string{ userId };

  Sqlite3Utils::TryReadText(stmt.get(), user.login, 0);
  Sqlite3Utils::TryReadText(stmt.get(), user.tag, 1);
  Sqlite3Utils::TryReadText(stmt.get(), user.public_key, 2);

  Sqlite3Utils::TryReadTimeStamp(stmt.get(), user.cached_at, 3);

  Sqlite3Utils::TryReadBool(stmt.get(), user.is_deleted, 4);

  return user;
}

void Sqlite3LocalDatabase::UpsertChat(const CachedChat& chat) {
  constexpr auto sql = R"(
    INSERT OR REPLACE INTO cached_chats (id, name, last_message_id, last_activity_at, cached_at, is_deleted)
    VALUES (?, ?, ?, ?, ?, ?);
  )";

  sqlite3_stmt* raw_stmt = nullptr;

  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Prepare failed: UpsertChat");
  }

  const Sqlite3Utils::ScopedStmt stmt{ raw_stmt };

  BindChat(stmt.get(), chat);

  if (!Sqlite3Utils::TryExecuteStore(stmt.get())) {
    throw std::runtime_error("Execute failed: UpsertChat");
  }
}

std::optional<CachedChat> Sqlite3LocalDatabase::LoadChat(std::string_view chatId) {
  constexpr auto sql = "SELECT name, last_message_id, last_activity_at, cached_at, is_deleted FROM cached_chats WHERE id = ?;";

  sqlite3_stmt* raw_stmt = nullptr;

  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Prepare failed: LoadChat");
  }

  Sqlite3Utils::ScopedStmt stmt{ raw_stmt };

  Sqlite3Utils::BindStr(stmt.get(), chatId, 1);

  if (!Sqlite3Utils::TryExecuteLoad(stmt.get())) {
    return std::nullopt;
  }

  CachedChat chat;
  chat.id = std::string{ chatId };

  Sqlite3Utils::TryReadOptionalText(stmt.get(), chat.name, 0);
  Sqlite3Utils::TryReadOptionalText(stmt.get(), chat.last_message_id, 1);

  Sqlite3Utils::TryReadTimeStamp(stmt.get(), chat.last_activity_at, 2);
  Sqlite3Utils::TryReadTimeStamp(stmt.get(), chat.cached_at, 3);

  Sqlite3Utils::TryReadBool(stmt.get(), chat.is_deleted, 4);

  return chat;
}

std::vector<CachedChat> Sqlite3LocalDatabase::LoadChats(std::size_t limit, std::size_t offset) {
  constexpr auto sql = R"(
    SELECT id, name, last_message_id, last_activity_at, cached_at, is_deleted
    FROM cached_chats
    WHERE is_deleted = 0
    ORDER BY cached_at DESC
    LIMIT ? OFFSET ?;
  )";

  sqlite3_stmt* raw_stmt;
  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Prepare failed: LoadChats");
  }

  Sqlite3Utils::ScopedStmt stmt{ raw_stmt };

  Sqlite3Utils::BindULL(stmt.get(), limit, 1);
  Sqlite3Utils::BindULL(stmt.get(), offset, 2);

  std::vector<CachedChat> chats;

  while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
    chats.emplace_back(ReadChatFromPreparedStmt(stmt.get()));
  }

  return chats;
}

void Sqlite3LocalDatabase::AddUserToChat(std::string_view userId, std::string_view chatId, const UserRoleVariant& chatRole) {  
  constexpr auto sql = R"(
    INSERT OR REPLACE INTO cached_chat_participants (chat_id, user_id, role, is_deleted)
    VALUES (?, ?, ?, 0);
  )";

  sqlite3_stmt* raw_stmt = nullptr;
  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Failed to prepare: AddUserToChat");
  }

  Sqlite3Utils::ScopedStmt stmt{ raw_stmt };
  
  Sqlite3Utils::BindStr(stmt.get(), chatId, 1);
  Sqlite3Utils::BindStr(stmt.get(), userId, 2);
  Sqlite3Utils::BindStr(stmt.get(), UserRoles::RoleToStrView(chatRole), 3);

  if (!Sqlite3Utils::TryExecuteStore(stmt.get())) {
    throw std::runtime_error("Execute failed: AddUserToChat");
  }
}

void Sqlite3LocalDatabase::SetUserChatRole(std::string_view userId, std::string_view chatId, const UserRoleVariant& chatRole) {
  constexpr auto sql = "UPDATE cached_chat_participants SET role = ? WHERE is_deleted = 0 AND user_id = ? AND chat_id = ?;";

  sqlite3_stmt* raw_stmt = nullptr;
  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Failed to prepare: SetUserChatRole");
  }

  Sqlite3Utils::ScopedStmt stmt{ raw_stmt };

  Sqlite3Utils::BindStr(stmt.get(), UserRoles::RoleToStrView(chatRole), 1);
  Sqlite3Utils::BindStr(stmt.get(), userId, 2);
  Sqlite3Utils::BindStr(stmt.get(), chatId, 3);

  if (!Sqlite3Utils::TryExecuteStore(stmt.get())) {
    throw std::runtime_error("Execute failed: SetUserChatRole");
  }
}

std::optional<UserRoleVariant> Sqlite3LocalDatabase::GetUserChatRole(std::string_view userId, std::string_view chatId) {
  constexpr auto sql = R"(
    SELECT role FROM cached_chat_participants
    WHERE is_deleted = 0
    AND user_id = ?
    AND chat_id = ?
  )";

  sqlite3_stmt* raw_stmt = nullptr;
  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Failed to prepare: GetUserChatRole");
  }

  Sqlite3Utils::ScopedStmt stmt{ raw_stmt };

  Sqlite3Utils::BindStr(stmt.get(), userId, 1);
  Sqlite3Utils::BindStr(stmt.get(), chatId, 2);

  if (!Sqlite3Utils::TryExecuteLoad(stmt.get())) {
    throw std::runtime_error("Execute failed: GetUserChatRole");
  }

  std::optional<std::string> role_str;

  Sqlite3Utils::TryReadOptionalText(stmt.get(), role_str, 0);

  if (role_str.has_value()) {
    return UserRoles::StringToRole(*role_str);
  }

  return std::nullopt;
}

std::vector<std::string> Sqlite3LocalDatabase::GetChatParticipantIds(std::string_view chatId) {
  constexpr auto sql = "SELECT user_id FROM cached_chat_participants WHERE is_deleted = 0 AND chat_id = ?;";

  sqlite3_stmt* raw_stmt = nullptr;
  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Prepare failed: GetChatParticipantIds");
  }

  const Sqlite3Utils::ScopedStmt stmt{ raw_stmt };

  Sqlite3Utils::BindStr(stmt.get(), chatId, 1);

  std::vector<std::string> participant_ids;

  while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
    participant_ids.emplace_back(ReadStringFromPreparedStmt(stmt.get()));
  }

  return participant_ids;
}

void Sqlite3LocalDatabase::SaveMessageForChat(const CachedMessage& message) {
  constexpr auto sql = R"(
    INSERT OR REPLACE INTO cached_messages 
    (id, chat_id, sender_id, plaintext, type, created_at, is_read, is_delivered, is_deleted)
    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);
  )";


  sqlite3_stmt* raw_stmt = nullptr;
  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Prepare failed: SaveMessageForChat");
  }

  const Sqlite3Utils::ScopedStmt stmt{ raw_stmt };

  BindMessage(stmt.get(), message);

  if (!Sqlite3Utils::TryExecuteStore(stmt.get())) {
    throw std::runtime_error("Execute failed: SaveMessageForChat");
  }

}

std::vector<CachedMessage> Sqlite3LocalDatabase::GetMessagesForChat(std::string_view chatId, std::size_t limit, std::size_t offset) {
  constexpr auto sql = R"(
    SELECT id, sender_id, plaintext, type, created_at, is_read, is_delivered, is_deleted
    FROM cached_messages 
    WHERE chat_id = ? 
    ORDER BY created_at DESC 
    LIMIT ? OFFSET ?;
  )";

  sqlite3_stmt* raw_stmt = nullptr;
  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Prepare failed: GetMessagesForChat");
  }

  const Sqlite3Utils::ScopedStmt stmt{ raw_stmt };

  Sqlite3Utils::BindStr(stmt.get(), chatId, 1);

  Sqlite3Utils::BindULL(stmt.get(), limit, 2);
  Sqlite3Utils::BindULL(stmt.get(), offset, 3);

  std::vector<CachedMessage> messages;

  while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
    messages.emplace_back(ReadMessageFromPreparedStmt(stmt.get(), chatId));
  }

  return messages;
}

void Sqlite3LocalDatabase::MarkMessageAsRead(std::string_view messageId) {
  constexpr auto sql = "UPDATE cached_messages SET is_read = 1 WHERE id = ?;";

  sqlite3_stmt* raw_stmt = nullptr;
  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Prepare failed: MarkMessageAsRead");
  }

  const Sqlite3Utils::ScopedStmt stmt{ raw_stmt };

  Sqlite3Utils::BindStr(stmt.get(), messageId, 1);

  if (!Sqlite3Utils::TryExecuteStore(stmt.get())) {
    throw std::runtime_error("Execute failed: MarkMessageAsRead");
  }
}

void Sqlite3LocalDatabase::EvictOldMessages(std::chrono::system_clock::time_point olderThanTimeStamp) {
  constexpr auto sql = "DELETE FROM cached_messages WHERE created_at < ?;";

  sqlite3_stmt* raw_stmt = nullptr;
  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Prepare failed: EvictOldMessages");
  }

  const Sqlite3Utils::ScopedStmt stmt{ raw_stmt };

  Sqlite3Utils::BindTimestamp(stmt.get(), olderThanTimeStamp, 1);

  if (!Sqlite3Utils::TryExecuteStore(stmt.get())) {
    throw std::runtime_error("Execute failed: EvictOldMessages");
  }
}

void Sqlite3LocalDatabase::NukeEntireCache() {
  constexpr auto sql = R"(
    DELETE FROM identity;
    DELETE FROM cached_users;
    DELETE FROM cached_chats;
    DELETE FROM cached_chat_participants;
    DELETE FROM cached_messages;
  )";

  if (char* err_msg = nullptr; !Sqlite3Utils::TryExecute(db_obs_, sql, err_msg)) {
    std::string err_str = err_msg 
      ? err_msg
      : "Unknown error";

    if (err_msg) {
      sqlite3_free(err_msg);
    }

    throw std::runtime_error(std::format("Failed to nuke cache: {}", err_str));
  }
}

void Sqlite3LocalDatabase::DeleteMessage(std::string_view messageId) {
  constexpr auto sql = "UPDATE cached_messages SET is_deleted = 1 WHERE id = ?;";

  sqlite3_stmt* raw_stmt = nullptr;
  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Prepare failed: DeleteMessage");
  }

  const Sqlite3Utils::ScopedStmt stmt{ raw_stmt };

  Sqlite3Utils::BindStr(stmt.get(), messageId, 1);

  if (!Sqlite3Utils::TryExecuteStore(stmt.get())) {
    throw std::runtime_error("Execute failed: DeleteMessage");
  }
}

void Sqlite3LocalDatabase::DeleteUser(std::string_view userId) {
  constexpr auto sql = "UPDATE cached_users SET is_deleted = 1 WHERE id = ?;";

  sqlite3_stmt* raw_stmt = nullptr;
  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Prepare failed: DeleteUser");
  }

  const Sqlite3Utils::ScopedStmt stmt{ raw_stmt };

  Sqlite3Utils::BindStr(stmt.get(), userId, 1);

  if (!Sqlite3Utils::TryExecuteStore(stmt.get())) {
    throw std::runtime_error("Execute failed: DeleteUser");
  }
}

void Sqlite3LocalDatabase::DeleteChat(std::string_view chatId) {
  constexpr auto sql = "UPDATE cached_chats SET is_deleted = 1 WHERE id = ?;";

  sqlite3_stmt* raw_stmt = nullptr;
  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Prepare failed: DeleteChat");
  }

  const Sqlite3Utils::ScopedStmt stmt{ raw_stmt };

  Sqlite3Utils::BindStr(stmt.get(), chatId, 1);

  if (!Sqlite3Utils::TryExecuteStore(stmt.get())) {
    throw std::runtime_error("Execute failed: DeleteChat");
  }
}

void Sqlite3LocalDatabase::DeleteUserFromChat(std::string_view userId, std::string_view chatId) {
  constexpr auto sql = "UPDATE cached_chat_participants SET is_deleted = 1 WHERE user_id = ? AND chat_id = ?;";

  sqlite3_stmt* raw_stmt = nullptr;
  if (!Sqlite3Utils::TryPrepare(db_obs_, sql, &raw_stmt)) {
    throw std::runtime_error("Prepare failed: DeleteUserFromChat");
  }

  const Sqlite3Utils::ScopedStmt stmt{ raw_stmt };

  Sqlite3Utils::BindStr(stmt.get(), userId, 1);
  Sqlite3Utils::BindStr(stmt.get(), chatId, 2);

  if (!Sqlite3Utils::TryExecuteStore(stmt.get())) {
    throw std::runtime_error("Execute failed: DeleteUserFromChat");
  }
}

CachedMessage Sqlite3LocalDatabase::ReadMessageFromPreparedStmt(sqlite3_stmt* rawStmt, std::string_view chatId) {
  CachedMessage msg;
  msg.chat_id = std::string(chatId);

  Sqlite3Utils::TryReadText(rawStmt, msg.id, 0);
  Sqlite3Utils::TryReadText(rawStmt, msg.sender_id, 1);
  Sqlite3Utils::TryReadText(rawStmt, msg.plaintext, 2);
  Sqlite3Utils::TryReadText(rawStmt, msg.type, 3);

  Sqlite3Utils::TryReadTimeStamp(rawStmt, msg.created_at, 4);

  Sqlite3Utils::TryReadBool(rawStmt, msg.is_read, 5);
  Sqlite3Utils::TryReadBool(rawStmt, msg.is_delivered, 6);
  Sqlite3Utils::TryReadBool(rawStmt, msg.is_deleted, 7);

  return msg;
}


CachedChat Sqlite3LocalDatabase::ReadChatFromPreparedStmt(sqlite3_stmt* rawStmt) {
  CachedChat chat;

  Sqlite3Utils::TryReadText(rawStmt, chat.id, 0);

  Sqlite3Utils::TryReadOptionalText(rawStmt, chat.name, 1);
  Sqlite3Utils::TryReadOptionalText(rawStmt, chat.last_message_id, 2);

  Sqlite3Utils::TryReadTimeStamp(rawStmt, chat.last_activity_at, 3);
  Sqlite3Utils::TryReadTimeStamp(rawStmt, chat.cached_at, 4);

  Sqlite3Utils::TryReadBool(rawStmt, chat.is_deleted, 5);

  
  return chat;
}

std::string Sqlite3LocalDatabase::ReadStringFromPreparedStmt(sqlite3_stmt* rawStmt) {
  std::string str;

  Sqlite3Utils::TryReadText(rawStmt, str, 0);

  return str;
}

void Sqlite3LocalDatabase::BindUser(sqlite3_stmt* rawStmt, const CachedUser& user) {
    Sqlite3Utils::BindStr(rawStmt, user.id, 1);
    Sqlite3Utils::BindStr(rawStmt, user.login, 2);
    Sqlite3Utils::BindStr(rawStmt, user.tag, 3);
    Sqlite3Utils::BindStr(rawStmt, user.public_key, 4);

    Sqlite3Utils::BindTimestamp(rawStmt, user.cached_at, 5);

    Sqlite3Utils::BindBool(rawStmt, user.is_deleted, 6);    
}

void Sqlite3LocalDatabase::BindChat(sqlite3_stmt* rawStmt, const CachedChat& chat) {
  Sqlite3Utils::BindStr(rawStmt, chat.id, 1);

  Sqlite3Utils::BindOptional(rawStmt, chat.name, 2);
  Sqlite3Utils::BindOptional(rawStmt, chat.last_message_id, 3);


  Sqlite3Utils::BindTimestamp(rawStmt, chat.last_activity_at, 4);
  Sqlite3Utils::BindTimestamp(rawStmt, chat.cached_at, 5);

  Sqlite3Utils::BindBool(rawStmt, chat.is_deleted, 6);
}

void Sqlite3LocalDatabase::BindMessage(sqlite3_stmt* rawStmt, const CachedMessage& msg) {
    Sqlite3Utils::BindStr(rawStmt, msg.id, 1);
    Sqlite3Utils::BindStr(rawStmt, msg.chat_id, 2);
    Sqlite3Utils::BindStr(rawStmt, msg.sender_id, 3);
    Sqlite3Utils::BindStr(rawStmt, msg.plaintext, 4);
    Sqlite3Utils::BindStr(rawStmt, msg.type, 5);

    Sqlite3Utils::BindTimestamp(rawStmt, msg.created_at, 6);

    Sqlite3Utils::BindBool(rawStmt, msg.is_read, 7);
    Sqlite3Utils::BindBool(rawStmt, msg.is_delivered, 8);
    Sqlite3Utils::BindBool(rawStmt, msg.is_deleted, 9);

}

