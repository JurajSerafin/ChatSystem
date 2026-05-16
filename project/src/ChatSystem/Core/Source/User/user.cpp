#include "User/user.h"

#include <variant>

bool User::CanPerform(UserAction action) const {
  return std::visit(
    [action](auto&& role) { return role.CanPerform(action); }
    , role_
  );
}

std::string User::GetRoleStr() const {
  return UserRoles::RoleToString(role_);
}
