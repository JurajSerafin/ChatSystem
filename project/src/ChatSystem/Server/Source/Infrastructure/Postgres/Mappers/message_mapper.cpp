#include "Infrastructure/Postgres/Mappers/message_mapper.h"

#include "Database/i_row.h"
#include "Message/Payloads/message_payload_factory.h"
#include "Message/message.h"

#include <Session/session.h>
#include <utility>

namespace {
  constexpr auto kIdColumn = "id";
  constexpr auto kChatIdColumn = "chat_id";
  constexpr auto kTypeColumn = "type";
  constexpr auto kContentColumn = "content";
  constexpr auto kSenderIdColumn = "sender_id";
  constexpr auto kCreatedAtColumn = "created_at";
}

Message MessageMapper::Map(const IRow& row) {

  MessageParams params{
    .id = MessageId::FromString(row.GetUuid(kIdColumn)),
    .chat_id = ChatId::FromString(row.GetUuid(kChatIdColumn)),
    .sender_id = UserId::FromString(row.GetUuid(kSenderIdColumn)),
    .payload = MessagePayloadFactory<MessagePayloadVariant>::Create(
      row.GetString(kTypeColumn), row.GetString(kContentColumn)
    ),
    .created_at = row.GetTimeStamp(kCreatedAtColumn),
  };

  return Message::Reconstitute(std::move(params));
}