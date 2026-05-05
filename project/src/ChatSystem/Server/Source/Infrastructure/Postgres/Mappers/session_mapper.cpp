#include "Infrastructure/Postgres/Mappers/user_mapper.h"

#include <Database/db_hydration_validator.h>
#include <Infrastructure/Postgres/Mappers/session_mapper.h>
#include <Session/session.h>
#include <utility>

namespace {
  constexpr auto kIdColumn = "id";
  constexpr auto kUserIdColumn = "user_id";
  constexpr auto kTokenColumn = "token";
  constexpr auto kCreatedAtColumn = "created_at";
  constexpr auto kExpiresAtColumn = "expires_at";
}

Session SessionMapper::Map(const IRow& row) {

  SessionParams params{
    .id = SessionId{row.GetUuid(kIdColumn)},
    .user_id = UserId::FromString(row.GetUuid(kUserIdColumn)),
    .token = row.GetString(kTokenColumn),
    .expires_at = row.GetTimeStamp(kCreatedAtColumn),
    .created_at = row.GetTimeStamp(kExpiresAtColumn),
  };

  return Session::Reconstitute(std::move(params));
}