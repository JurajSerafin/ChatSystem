#ifndef REGULAR_USER_ROLE_H
#define REGULAR_USER_ROLE_H


#include <set>
#include <string>

#include <User/user_action.h>
#include <User/i_user_role.h>

class RegularUserRole : public IUserRole {
private:
    inline static const std::set<UserAction> kPerformableActions{
        UserAction::kSendMessage,
        UserAction::kDeleteAccount,
        UserAction::kDeleteOwnMessage,
        UserAction::kModifyLogin,
        UserAction::kModifyPassword,
    };

    static constexpr std::string kRoleName = "REGULAR_USER";

public:
    [[nodiscard]] const std::string& RoleName() const override;

    [[nodiscard]] bool CanPerform(UserAction action) const override;

    RegularUserRole() = default;

    RegularUserRole(const RegularUserRole& other) = default;

    RegularUserRole(RegularUserRole&& other) = default;

    RegularUserRole& operator=(const RegularUserRole& other) = delete;
    RegularUserRole& operator=(RegularUserRole&& other) = delete;

    ~RegularUserRole() override = default;
};

inline const std::string& RegularUserRole::RoleName() const {
    return kRoleName;
}

inline bool RegularUserRole::CanPerform(UserAction action) const {
    return kPerformableActions.contains(action);
}

#endif  // REGULAR_USER_ROLE_H