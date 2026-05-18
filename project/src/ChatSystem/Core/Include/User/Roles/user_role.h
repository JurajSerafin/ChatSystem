#ifndef USER_ROLE_H
#define USER_ROLE_H

#include "User/user_action.h"

#include <concepts>
#include <string_view>
#include <type_traits>

/**
 * @brief Concept defining the strict structural requirements for a User Role.
 * * Any class functioning as a role within the application's Role-Based Access Control
 * system must satisfy this concept to ensure compile-time safety and polymorphic behavior.
 * * @tparam TUserRole The type being evaluated against the concept.
 */
template<typename TUserRole>
concept UserRole = requires(TUserRole role, UserAction userAction) {

  /** @brief Must provide a static string identifier for the role type (e.g., "ADMIN"). */
  { TUserRole::TypeString() } -> std::convertible_to<std::string_view>;

  /** @brief Must evaluate whether this role has permission to execute a specific action. */
  { role.CanPerform(userAction) } -> std::same_as<bool>;
};

#endif // USER_ROLE_H