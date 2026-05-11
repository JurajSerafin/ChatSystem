#include <Infrastructure/Postgres/Mappers/chat_mapper.h>

#include "Database/i_row.h"
#include "TypeLibHelpers/domain_class_type_variant_factory.h"

#include <optional>
#include <string>
#include <utility>

namespace {
  constexpr auto kIdColumn = "id";
  constexpr auto kTypeColumn = "type";
  constexpr auto kCreatedAtColumn = "created_at";

  constexpr const char* kMsgIdColumn = "msg_id";
  constexpr const char* kMsgSenderIdColumn = "msg_sender_id";
  constexpr const char* kMsgCiphertextColumn = "msg_ciphertext";
  constexpr auto kMsgTypeColumn = "msg_type";
  constexpr const char* kMsgCreatedAtColumn = "msg_created_at";
}

Chat ChatMapper::Map(const IRow& row, std::vector<UserId> participants) {

  std::optional<Message> last_message = std::nullopt;

  if (!row.IsNull(kMsgIdColumn)) {

    MessageParams msg_params{
      .id = MessageId::Reconstitute(row.GetUuid(kMsgIdColumn)),
      .chat_id = ChatId::Reconstitute(row.GetUuid(kIdColumn)),
      .sender_id = UserId::Reconstitute(row.GetUuid(kMsgSenderIdColumn)),
      .ciphertext = row.GetString(kMsgCiphertextColumn),
      .type = DomainClassTypeVariantFactory<MessageTypeVariant>::Create(row.GetString(kMsgTypeColumn)),
      .created_at = row.GetTimeStamp(kMsgCreatedAtColumn),
    };

    last_message = Message::Reconstitute(std::move(msg_params));
  }

  ChatParams params{
    .id = ChatId::Reconstitute(row.GetUuid(kIdColumn)),
    .created_at = row.GetTimeStamp(kCreatedAtColumn),
    .type = DomainClassTypeVariantFactory<ChatTypeVariant>::Create(row.GetString(kTypeColumn)),
    .last_message = std::move(last_message),
    .participant_ids = std::move(participants)
  };

  return Chat::Reconstitute(std::move(params));
}