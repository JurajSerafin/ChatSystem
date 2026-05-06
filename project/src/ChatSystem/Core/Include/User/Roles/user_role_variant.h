#ifndef USER_ROLE_VARIANT_H
#define USER_ROLE_VARIANT_H

#include "regular_user_role.h"
#include "user_role.h"
#include "user_role_validator.h"

#include <variant>

using UserRoleVariant = std::variant<
 RegularUserRole
>;

static_assert(AllUserRolesValid<UserRoleVariant>, "Role not valid, make sure it is compliant with UserRole concept.");

#endif // USER_ROLE_VARIANT_H