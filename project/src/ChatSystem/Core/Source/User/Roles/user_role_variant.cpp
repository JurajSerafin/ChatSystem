#include "User/Roles/user_role_variant.h"

#include "TypeLibHelpers/domain_class_type_variant_factory.h"

#include <string>
#include <variant>

namespace UserRoles {
  std::string RoleToString(const UserRoleVariant& roleVar) {
    return std::visit(
      [&](auto&& role) ->std::string { return role.TypeString(); },
      roleVar
    );
  }

  UserRoleVariant StringToRole(std::string_view roleStr) {
    return DomainClassTypeVariantFactory<UserRoleVariant>::Create(roleStr);
  }
} // UserRoles