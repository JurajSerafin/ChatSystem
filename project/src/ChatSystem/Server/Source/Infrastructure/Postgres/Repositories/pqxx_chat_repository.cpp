#include "Chat/chat.h"

#include <Database/query_params.h>
#include <Database/transaction.h>
#include <Infrastructure/Postgres/Mappers/chat_mapper.h>
#include <Infrastructure/Postgres/Repositories/pqxx_chat_repository.h>
#include <stdexcept>
#include <utility>

PqxxChatRepository::PqxxChatRepository(IConnectionPool* connectionPoolObs)
  : connection_pool_obs_(connectionPoolObs) {
  if (!connection_pool_obs_) {
    throw std::invalid_argument("PqxxChatRepository requires IConnectionPool");
  }
}

void PqxxChatRepository::Add(const Chat& chat) {
  auto tx = Transaction{ std::move(connection_pool_obs_->Acquire()) };

  const auto chat_type = std::visit(
    [](auto&& chatTypeVar) { return std::string(chatTypeVar.TypeString()); },
    chat.GetType()
  );

  const std::string chat_sql = R"(
    INSERT INTO chats (id, type, created_at) 
    VALUES ($1, $2, $3) RETURNING *
  )";

  const auto params = QueryParams{}
    .BindParam(chat.GetId().ToString())
    .BindParam(chat_type)
    .BindParam(chat.CreatedAt());

  tx.Execute(chat_sql, params);

  const std::string part_sql = "INSERT INTO chat_participants (chat_id, user_id) VALUES ($1, $2)";

  for (const auto& user_id : chat.GetParticipantIds()) {
    tx.Execute(part_sql, QueryParams{}.BindParam(chat.GetId().ToString()).BindParam(user_id.ToString()));
  }

  tx.Commit();
}

std::optional<Chat> PqxxChatRepository::FindById(const ChatId& id) {
  auto tx = Transaction{ connection_pool_obs_->Acquire() };

  const std::string sql = R"(
    SELECT c.*, 
      m.id AS msg_id, m.sender_id AS msg_sender_id, 
      m.type AS msg_type, m.ciphertext AS msg_ciphertext, 
      m.created_at AS msg_created_at,
      (SELECT STRING_AGG(user_id::text, ',') 
      FROM chat_participants 
      WHERE chat_id = c.id AND left_at IS NULL) 
      AS participants_csv
    FROM chats c
    LEFT JOIN messages m ON c.last_message_id = m.id
    WHERE c.id = $1
  )";

  const auto fetched_chat_result = tx.Execute(sql, QueryParams{}.BindParam(id.ToString()));

  if (fetched_chat_result->IsEmpty()) {
    return std::nullopt;
  }

  std::optional<Chat> fetched_chat;

  fetched_chat_result->First([&fetched_chat](const IRow& row) {
    std::vector<UserId> participants;
    std::stringstream ss(row.GetString("participants_csv"));
    std::string token;
    while (std::getline(ss, token, ',')) {
      participants.push_back(UserId::Reconstitute(token));
    }

    fetched_chat = ChatMapper::Map(row, std::move(participants));
  });

  return fetched_chat;
}

std::vector<Chat> PqxxChatRepository::FindByUserId(const UserId& userId, std::size_t limit, std::size_t offset) {
  auto tx = Transaction{ connection_pool_obs_->Acquire() };

  const std::string sql = R"(
    SELECT c.*, 
      m.id AS msg_id, m.sender_id AS msg_sender_id, 
      m.type AS msg_type, m.ciphertext AS msg_ciphertext, 
      m.created_at AS msg_created_at,
      (SELECT STRING_AGG(user_id::text, ',') 
      FROM chat_participants 
      WHERE chat_id = c.id AND left_at IS NULL) AS participants_csv
    FROM chats c
    JOIN chat_participants cp ON c.id = cp.chat_id
    LEFT JOIN messages m ON c.last_message_id = m.id
    WHERE cp.user_id = $1 AND cp.left_at IS NULL
    ORDER BY c.last_message_timestamp DESC NULLS LAST
    LIMIT $2 OFFSET $3
  )";

  const auto fetched_chats_result = tx.Execute(sql, QueryParams{}
    .BindParam(userId.ToString())
    .BindParam(limit)
    .BindParam(offset));

  std::vector<Chat> chats;

  fetched_chats_result->ForEach([&](const IRow& row) {
    std::vector<UserId> participants;
    std::stringstream ss(row.GetString("participants_csv"));
    std::string token;

    while (std::getline(ss, token, ',')) {
      participants.push_back(UserId::Reconstitute(token));
    }

    chats.push_back(ChatMapper::Map(row, std::move(participants)));
  });

  return chats;
}

void PqxxChatRepository::AddParticipant(const ChatId& chatId, const UserId& userId) {
  auto tx = Transaction{ connection_pool_obs_->Acquire() };

  const std::string sql = R"(
    INSERT INTO chat_participants (chat_id, user_id) 
    VALUES ($1, $2)
  )";

  tx.Execute(sql, QueryParams{}
    .BindParam(chatId.ToString())
    .BindParam(userId.ToString())
  );

  tx.Commit();
}

void PqxxChatRepository::UpdateLastMessage(const ChatId& chatId, const Message& message) {
  auto tx = Transaction{ connection_pool_obs_->Acquire() };

  tx.Execute("UPDATE chats SET last_message_id = $1, last_message_timestamp = $2 WHERE id = $3",
    QueryParams{}.BindParam(message.GetId().ToString())
    .BindParam(message.CreatedAt())
    .BindParam(chatId.ToString()));

  tx.Commit();
}

bool PqxxChatRepository::IsParticipant(const ChatId& chatId, const UserId& userId) {
  auto tx = Transaction{connection_pool_obs_->Acquire() };

  const auto fetched_participants_result = tx.Execute("SELECT 1 FROM chat_participants WHERE chat_id = $1 AND user_id = $2 LIMIT 1",
    QueryParams{}.BindParam(chatId.ToString()).BindParam(userId.ToString()));

  return !fetched_participants_result->IsEmpty();
}

void PqxxChatRepository::DeleteById(const ChatId& id) {
  auto tx = Transaction{ connection_pool_obs_->Acquire() };

  tx.Execute("DELETE FROM chats WHERE id = $1", QueryParams{}.BindParam(id.ToString()));

  tx.Commit();
}

void PqxxChatRepository::RemoveParticipant(const ChatId& chatId, const UserId& userId) {
  auto tx = Transaction{ connection_pool_obs_->Acquire() };

  const std::string sql = R"(
    UPDATE chat_participants 
    SET left_at = CURRENT_TIMESTAMP 
    WHERE chat_id = $1 AND user_id = $2 AND left_at IS NULL
  )";

  tx.Execute(sql, QueryParams{}
    .BindParam(chatId.ToString())
    .BindParam(userId.ToString())
  );

  tx.Commit();
}