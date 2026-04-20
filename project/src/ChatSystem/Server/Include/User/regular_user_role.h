#ifndef REGULAR_USER_ROLE_H
#define REGULAR_USER_ROLE_H

#include <set>
#include <string>

#include <User/user_action.h>
#include <User/i_user_role.h>

/**
 * @brief Concrete role representing a regular user.
 *
 * This role defines a fixed set of permitted user actions such as:
 * - Sending messages
 * - Deleting own account
 * - Modifying login and password
 *
 * Permissions are defined statically and cannot be modified at runtime.
 */
class RegularUserRole : public IUserRole {
private:
  /**
   * @brief Set of actions permitted for regular users.
   *
   * This collection defines all operations that a regular user
   * is allowed to perform within the system.
   */
  inline static const std::set<UserAction> kPerformableActions{
      UserAction::kSendMessage,
      UserAction::kDeleteAccount,
      UserAction::kDeleteOwnMessage,
      UserAction::kModifyLogin,
      UserAction::kModifyPassword,
  };

public:
  /**
   * @brief Returns the role name identifier.
   *
   * @return Constant reference to the string that represents RegularUserRole.
   */
  [[nodiscard]] const std::string& RoleName() const override;

  /**
   * @brief Checks whether the role allows a specific user action.
   *
   * @param action The action to evaluate.
   * @return true if the action is permitted, false otherwise.
   */
  [[nodiscard]] bool CanPerform(UserAction action) const override;

  /**
   * @brief Default constructor.
   */
  RegularUserRole() = default;

  /**
   * @brief Copy constructor.
   */
  RegularUserRole(const RegularUserRole& other) = default;

  /**
   * @brief Move constructor.
   */
  RegularUserRole(RegularUserRole&& other) = default;

  /**
   * @brief Deleted copy assignment operator.
   */
  RegularUserRole& operator=(const RegularUserRole& other) = delete;

  /**
   * @brief Deleted move assignment operator.
   */
  RegularUserRole& operator=(RegularUserRole&& other) = delete;

  /**
   * @brief Virtual destructor.
   */
  ~RegularUserRole() override = default;
};

inline const std::string& RegularUserRole::RoleName() const {
  static const std::string kRoleName = "REGULAR_USER";

  return kRoleName;
}

inline bool RegularUserRole::CanPerform(const UserAction action) const {
  return kPerformableActions.contains(action);
}

#endif  // REGULAR_USER_ROLE_H