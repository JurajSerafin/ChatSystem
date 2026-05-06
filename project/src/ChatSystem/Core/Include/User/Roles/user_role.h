#ifndef USER_ROLE_H
#define USER_ROLE_H

#include "User/user_action.h"

#include <concepts>
#include <string_view>
#include <type_traits>

template<typename TUserRole>
concept UserRole = requires(TUserRole role, UserAction userAction) {
  { TUserRole::TypeString() } -> std::convertible_to<std::string_view>;
  { role.CanPerform(userAction) } -> std::same_as<bool>;
};

#endif // USER_ROLE_H