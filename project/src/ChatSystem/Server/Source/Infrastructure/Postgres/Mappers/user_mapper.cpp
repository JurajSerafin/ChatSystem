#include <Infrastructure/Postgres/Mappers/user_mapper.h>

#include <User/user_role_factory.h>

#include <Database/db_hydration_validator.h>

#include <utility>

namespace {
  constexpr auto kIdColumn = "id";
  constexpr auto kTagColumn = "tag";
  constexpr auto kLoginColumn = "login";
  constexpr auto kPasswordHashColumn = "password_hash";
  constexpr auto kPublicKeyColumn = "public_key";
  constexpr auto kRoleColumn = "role";
  constexpr auto kCreatedAtColumn = "created_at";
}

User UserMapper::Map(const IRow& row) {
  
  UserParams params{
    .id = row.GetUuid(kIdColumn),
    .tag = row.GetString(kTagColumn),
    .login = row.GetString(kLoginColumn),
    .password_hash = row.GetString(kPasswordHashColumn),
    .public_key = row.GetString(kPublicKeyColumn),
    .role = UserRoleFactory<ActiveRoles>::Create(row.GetString(kRoleColumn)),
    .created_at = row.GetTimeStamp(kCreatedAtColumn)
  };

  return User::Create(std::move(params), DbHydrationValidator<UserParams>{});
}