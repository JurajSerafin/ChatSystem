#ifndef USER_ROLE_FACTORY_H
#define USER_ROLE_FACTORY_H

#include "i_user_role.h"

#include <memory>
#include <stdexcept>
#include <tuple>

template<typename TRoles>
class UserRoleFactory;

template<typename ... TRole>
class UserRoleFactory<std::tuple<TRole ...>> {
public:
  [[nodiscard]] static std::unique_ptr<IUserRole> Create(std::string_view roleName) {
    std::unique_ptr<IUserRole> created_role_ptr = nullptr;

    bool role_exists = (
      (TRole{}.RoleName() == roleName
        ? (created_role_ptr = std::make_unique<TRole>(), true)
        : false
      ) || ...);
     
    if (!role_exists) {
      throw std::runtime_error("Role that is being factoried is not present in active roles.");
    }

    return created_role_ptr;
  }
};

#endif // USER_ROLE_FACTORY_H
