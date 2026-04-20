#ifndef USER_ACTION_H
#define USER_ACTION_H

/**
 * @brief Enumerates all possible actions a user can perform in the system.
 *
 * This enum defines the full set of operations that can be checked
 * against user roles for authorization purposes.
 *
 * Each action represents a specific capability that may be granted
 * or denied depending on the user's role.
 */
enum class UserAction {
  /**
    * @brief Send a message to another user or chat.
    */
  kSendMessage,

  /**
    * @brief Delete the user's own account.
    */
  kDeleteAccount,

  /**
    * @brief Modify the user's login/username.
    */
  kModifyLogin,

  /**
    * @brief Modify the user's password.
    */
  kModifyPassword,

  /**
    * @brief Modify the user's tag/identifier.
    */
  kModifyTag,

  /**
    * @brief Delete a message owned by the user.
    */
  kDeleteOwnMessage,

  /**
    * @brief Delete any message regardless of ownership.
    */
  kDeleteAnyMessage,

  /**
    * @brief Ban a user from the system.
    */
  kBanUser,

  /**
    * @brief Manage user roles and permissions.
    */
  kManageRoles
};

#endif // USER_ACTION_H
