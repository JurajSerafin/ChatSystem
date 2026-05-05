#ifndef I_USER_ROLE_H
#define I_USER_ROLE_H

#include <string_view>

#include <User/user_action.h>

/**
 * @brief Interface representing a user role in the system.
 *
 * A user role defines:
 * - A unique role name (identifier)
 * - A set of permissions expressed as allowed actions
 *
 * Implementations of this interface define specific permission sets
 * and authorization rules for different user types.
 */
class IUserRole {
public:
  /**
   * @brief Returns the name of the role.
   *
   * The role name is a unique identifier used for role comparison,
   * persistence, and debugging.
   *
   * @return Constant reference to the role name string.
   */
  [[nodiscard]] virtual std::string_view RoleName() const = 0;

  /**
   * @brief Checks whether the role allows performing a specific action.
   *
   * @param action The action to check permission for.
   * @return true if the action is permitted for this role, false otherwise.
   */
  [[nodiscard]] virtual bool CanPerform(UserAction action) const = 0;

  /**
   * @brief Virtual destructor.
   *
   * Ensures proper cleanup of derived role implementations.
   */
  virtual ~IUserRole() = default;
};

#endif // I_USER_ROLE_H