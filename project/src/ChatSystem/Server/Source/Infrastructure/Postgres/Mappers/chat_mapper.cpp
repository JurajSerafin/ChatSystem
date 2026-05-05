#include <Infrastructure/Postgres/Mappers/chat_mapper.h>
#include <optional>
#include <string>
#include <utility>

namespace {
  constexpr const char* kIdColumn = "id";
  constexpr const char* kNameColumn = "name";
  constexpr const char* kCreatedAtColumn = "created_at";
}

Chat ChatMapper::Map(const IRow& row) {

  std::optional<std::string> chat_name = std::nullopt;

  if (!row.IsNull(kNameColumn)) {
    if (std::string fetched_name = row.GetString(kNameColumn); !fetched_name.empty()) {
      chat_name = std::move(fetched_name);
    }
  }

  ChatParams params{
    .id = ChatId::FromString(row.GetUuid(kIdColumn)),
    .created_at = row.GetTimeStamp(kCreatedAtColumn),
    .name = std::move(chat_name)
  };

  return Chat::Reconstitute(std::move(params));
}