#include "Infrastructure/Postgres/Mappers/message_mapper.h"

#include "Database/i_row.h"
#include "Message/message.h"
#include "TypeLibHelpers/domain_class_type_variant_factory.h"

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
    .id = MessageId::Reconstitute(row.GetUuid(kIdColumn)),
    .chat_id = ChatId::Reconstitute(row.GetUuid(kChatIdColumn)),
    .sender_id = UserId::Reconstitute(row.GetUuid(kSenderIdColumn)),
    .payload = DomainClassTypeVariantFactory<MessagePayloadVariant>::Create(
      row.GetString(kTypeColumn), row.GetString(kContentColumn)
    ),
    .created_at = row.GetTimeStamp(kCreatedAtColumn),
  };

  return Message::Reconstitute(std::move(params));
}