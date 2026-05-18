#include "Database/Postgres/Repositories/pqxx_message_repository.h"

#include "Message/encrypted_keys_map.h"
#include "Message/message.h"

#include <Database/query_params.h>
#include <Database/transaction.h>
#include "Database/Mappers/message_mapper.h"
#include <stdexcept>

PqxxMessageRepository::PqxxMessageRepository(IConnectionPool* connectionPoolObs)
  : connection_pool_obs_(connectionPoolObs) {
  if (!connection_pool_obs_) {
    throw std::invalid_argument("PqxxMessageRepository requires IConnectionPool");
  }
}

void PqxxMessageRepository::Add(const Message& message, const EncryptedKeysMap& encryptedKeys) {
  auto tx = Transaction{ connection_pool_obs_->Acquire() };

  const std::string msg_sql = "INSERT INTO messages (id, chat_id, sender_id, ciphertext, type, created_at) VALUES ($1, $2, $3, $4, $5, $6)";

  tx.Execute(msg_sql, QueryParams{}
    .BindParam(message.GetId().ToString())
    .BindParam(message.GetChatId().ToString())
    .BindParam(message.GetSenderId().ToString())
    .BindParam(message.GetCiphertext())
    .BindParam(message.GetTypeStr())
    .BindParam(message.CreatedAt())
  );

  const std::string keys_sql = "INSERT INTO message_keys (message_id, user_id, encrypted_key) VALUES ($1, $2, $3)";

  for (const auto& [user_id, key] : encryptedKeys) {
    tx.Execute(keys_sql, QueryParams{}
      .BindParam(message.GetId().ToString())
      .BindParam(user_id.ToString())
      .BindParam(key)
    );
  }

  tx.Commit();

}

std::optional<std::string> PqxxMessageRepository::GetEncryptedKey(const MessageId& messageId, const UserId& userId) {
  auto tx = Transaction{ connection_pool_obs_->Acquire() };

  const std::string sql = "SELECT encrypted_key FROM message_keys WHERE message_id = $1 AND user_id = $2";

  const auto fetched_encr_keys_result_result = tx.Execute(
    sql,
    QueryParams{}.BindParam(messageId.ToString()).BindParam(userId.ToString())
  );

  std::optional<std::string> encr_key;

  if (fetched_encr_keys_result_result->IsEmpty()) {
    return std::nullopt;
  }

  fetched_encr_keys_result_result->First([&encr_key](const IRow& row) {
    encr_key = row.GetString("encrypted_key");
  });

  return encr_key;
}

std::optional<Message> PqxxMessageRepository::FindById(const MessageId& id) {
  auto tx = Transaction{ connection_pool_obs_->Acquire() };

  const auto fetched_msg_result = tx.Execute("SELECT * FROM messages WHERE id = $1",
    QueryParams{}.BindParam(id.ToString()));

  if (fetched_msg_result->IsEmpty()) {
    return std::nullopt;
  }

  std::optional<Message> msg;

  fetched_msg_result->First([&msg](const IRow& row) { msg = MessageMapper::Map(row); });

  return msg;
}

void PqxxMessageRepository::DeleteById(const MessageId& id) {
  auto tx = Transaction{ connection_pool_obs_->Acquire() };

  tx.Execute("DELETE FROM messages WHERE id = $1",QueryParams{}.BindParam(id.ToString()));

  tx.Commit();
}


std::vector<Message> PqxxMessageRepository::FindByChatId(const ChatId& chatId, std::size_t limit, std::size_t offset) {
  auto tx = Transaction{ connection_pool_obs_->Acquire() };

  const std::string sql = "SELECT * FROM messages WHERE chat_id = $1 ORDER BY created_at DESC LIMIT $2 OFFSET $3";

  const auto fetched_msg_result = tx.Execute(sql, QueryParams{}
    .BindParam(chatId.ToString())
    .BindParam(limit)
    .BindParam(offset));

  std::vector<Message> messages;

  fetched_msg_result->ForEach([&messages](const IRow& row) { messages.push_back(MessageMapper::Map(row)); });

  return messages;
}


std::vector<Message> PqxxMessageRepository::FindUndelivered(const UserId& recipientId, std::size_t limit, std::optional<MessageId> afterMessageId) {
  auto tx = Transaction{ connection_pool_obs_->Acquire() };

  std::string sql = R"(
    SELECT m.* FROM messages m
    JOIN chat_participants cp ON m.chat_id = cp.chat_id
    LEFT JOIN message_receipts mr ON m.id = mr.message_id AND mr.user_id = cp.user_id
    WHERE cp.user_id = $1 
      AND m.sender_id != $1 
      AND mr.delivered_at IS NULL
  )";

  auto params = QueryParams{}
  .BindParam(recipientId.ToString());

  if (afterMessageId.has_value()) {
    params.BindParam(afterMessageId->ToString());
    params.BindParam(limit);

    sql += " AND m.created_at > (SELECT created_at from messages WHERE id = $2) ";

    sql += " ORDER BY m.created_at ASC LIMIT $3 ";
  }
  else {
    params.BindParam(limit);

    sql += "ORDER BY m.created_at ASC LIMIT $2 ";
  }

  const auto fetched_undelivered_result = tx.Execute(sql, params);

  std::vector<Message> messages;

  fetched_undelivered_result->ForEach([&messages](const IRow& row) { messages.push_back(MessageMapper::Map(row)); });

  return messages;
}

void PqxxMessageRepository::MarkDelivered(const MessageId& messageId, const UserId& recipientId) {
  auto tx = Transaction{ connection_pool_obs_->Acquire() };

  const std::string sql = R"(
    INSERT INTO message_receipts (message_id, user_id, delivered_at) 
    VALUES ($1, $2, CURRENT_TIMESTAMP)
    ON CONFLICT (message_id, user_id) 
    DO UPDATE SET delivered_at = CURRENT_TIMESTAMP 
    WHERE message_receipts.delivered_at IS NULL
  )";

  tx.Execute(sql, QueryParams{}
    .BindParam(messageId.ToString())
    .BindParam(recipientId.ToString()));
  tx.Commit();
}

void PqxxMessageRepository::MarkRead(const MessageId& messageId, const UserId& readerId) {
  auto tx = Transaction{ connection_pool_obs_->Acquire()};

  const std::string sql = R"(
    INSERT INTO message_receipts (message_id, user_id, delivered_at, read_at) 
    VALUES ($1, $2, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)
    ON CONFLICT (message_id, user_id) 
    DO UPDATE SET 
      read_at = CURRENT_TIMESTAMP,
      delivered_at = COALESCE(message_receipts.delivered_at, CURRENT_TIMESTAMP)
  )";

  tx.Execute(sql, QueryParams{}
    .BindParam(messageId.ToString())
    .BindParam(readerId.ToString()));
  tx.Commit();
}

std::vector<UserId> PqxxMessageRepository::GetDeliveredTo(const MessageId& messageId) {
  auto tx = Transaction{ connection_pool_obs_->Acquire() };

  const auto fetched_delivered_to_result = tx.Execute(
    "SELECT user_id FROM message_receipts WHERE message_id = $1 AND delivered_at IS NOT NULL",
    QueryParams{}.BindParam(messageId.ToString()));

  std::vector<UserId> delivered;

  fetched_delivered_to_result->ForEach([&delivered](const IRow& row) {
    delivered.push_back(UserId::Reconstitute(row.GetString("user_id")));
  });

  return delivered;
}

std::vector<UserId> PqxxMessageRepository::GetReaders(const MessageId& messageId) {
  auto tx = Transaction{ connection_pool_obs_->Acquire() };

  const auto fetched_readers_result = tx.Execute("SELECT user_id FROM message_receipts WHERE message_id = $1 AND read_at IS NOT NULL",
    QueryParams{}.BindParam(messageId.ToString()));

  std::vector<UserId> readers;

  fetched_readers_result->ForEach([&readers](const IRow& row) {
    readers.push_back(UserId::Reconstitute(row.GetString("user_id")));
  });

  return readers;
}
