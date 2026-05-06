#ifndef USER_ROLE_VALIDATOR_H
#define USER_ROLE_VALIDATOR_H

#include "user_role.h"

#include <variant>

template <typename TRoleVariant>
constexpr bool AllUserRolesValid = false;

template<typename ... TRoles>
constexpr bool AllUserRolesValid<std::variant<TRoles...>> = (UserRole<TRoles> && ...);

#endif // USER_ROLE_VALIDATOR_H