#ifndef REGULAR_USER_ROLE_H
#define REGULAR_USER_ROLE_H

#include <User/user_action.h>
#include <algorithm>
#include <array>
#include <string_view>

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
class RegularUserRole {
private:

  constexpr static std::size_t kPerformableActionsCount = 5;

  /**
   * @brief Set of actions permitted for regular users.
   *
   * This collection defines all operations that a regular user
   * is allowed to perform within the system.
   */
  constexpr static std::array<UserAction, kPerformableActionsCount> kPerformableActions {
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
  [[nodiscard]] constexpr static std::string_view TypeString();

  /**
   * @brief Checks whether the role allows a specific user action.
   *
   * @param action The action to evaluate.
   * @return true if the action is permitted, false otherwise.
   */
  [[nodiscard]] static bool CanPerform(UserAction action);
};

constexpr std::string_view RegularUserRole::TypeString() {
  return "REGULAR_USER";
}

inline bool RegularUserRole::CanPerform(const UserAction action) {
  return std::ranges::find(kPerformableActions, action) != kPerformableActions.end();
}

#endif  // REGULAR_USER_ROLE_H