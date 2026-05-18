#include <gtest/gtest.h>
#include <stdexcept>
#include <User/user.h>
#include <User/user_validator.h>
#include <User/user_params.h>
#include <User/user_id.h>
#include <Tags/user_tag.h>
#include <User/regular_user_role.h>

namespace user::tests {

  // Helper to generate a baseline valid params object
  static UserParams CreateValidUserParams() {
    return UserParams{
      .id = UserId::Generate(),
      .tag = tags::UserTag{"bob#1234"},
      .login = "bob_",
      .password_hash = "bob_1234",
      .public_key = "AAAAAAAAAAAAAAAA",
      .role = std::make_unique<RegularUserRole>()
    };
  }

  // ---------------------------------------------------------
  // Creation & Validation
  // ---------------------------------------------------------


  TEST(UserTest, CreateSucceedsWithValidParameters) {
    // Arrange
    UserValidator validator;
    UserParams params = CreateValidUserParams();

    // Act
    User user = User::Create(std::move(params), validator);

    // Assert
    EXPECT_EQ(user.GetLogin(), "bob_");
    EXPECT_EQ(user.GetPasswordHash(), "bob_1234");
  }

  TEST(UserTest, CreateFailsWhenLoginIsTooShort) {
    // Arrange
    UserValidator validator;
    UserParams params = CreateValidUserParams();
    params.login = "X"; // kMinLoginLength = 3

    // Act & Assert
    EXPECT_THROW({
      User::Create(std::move(params), validator);
      }, std::invalid_argument
    );
  }

  TEST(UserTest, CreateFailsWhenLoginIsTooLong) {
    // Arrange
    UserValidator validator;
    UserParams params = CreateValidUserParams();
    params.login = std::string(33, 'a'); // kMaxLoginLength = 32

    // Act & Assert
    EXPECT_THROW({
      User::Create(std::move(params), validator);
      }, std::invalid_argument
    );
  }

  TEST(UserTest, CreateFailsWhenPasswordHashIsEmpty) {
    // Arrange
    UserValidator validator;
    UserParams params = CreateValidUserParams();
    params.password_hash = "";

    // Act & Assert
    EXPECT_THROW({
      User::Create(std::move(params), validator);
      }, std::invalid_argument
    );
  }
  
  // ---------------------------------------------------------
  // Mutation
  // ---------------------------------------------------------

  TEST(UserTest, SetLoginUpdatesValueWhenValid) {
    // Arrange
    UserValidator validator;
    User user = User::Create(CreateValidUserParams(), validator);
    const std::string new_login = "alice";

    // Act
    user.SetLogin(new_login);

    // Assert
    EXPECT_EQ(user.GetLogin(), new_login);
  }


  TEST(UserTest, SetPasswordHashUpdatesValueWhenValid) {
    // Arrange
    UserValidator validator;
    User user = User::Create(CreateValidUserParams(), validator);
    const std::string new_hash = "new_hash_123";

    // Act
    user.SetPasswordHash(new_hash);

    // Assert
    EXPECT_EQ(user.GetPasswordHash(), new_hash);
  }

  // ---------------------------------------------------------
  // Role & Permissions
  // ---------------------------------------------------------

  TEST(UserTest, GetRoleReturnsAssignedRoleCorrectly) {
    // Arrange
    UserValidator validator;
    User user = User::Create(CreateValidUserParams(), validator);

    // Act
    const auto& role = user.GetRole();

    // Assert
    EXPECT_EQ(role.RoleName(), "REGULAR_USER");
  }

  TEST(UserTest, CanPerformDelegatesToUnderlyingRole) {
    // Arrange
    UserValidator validator;
    User user = User::Create(CreateValidUserParams(), validator);

    // Act & Assert
    EXPECT_TRUE(user.CanPerform(UserAction::kSendMessage));
    EXPECT_TRUE(user.CanPerform(UserAction::kModifyLogin));
    EXPECT_TRUE(user.CanPerform(UserAction::kModifyPassword));
    EXPECT_TRUE(user.CanPerform(UserAction::kDeleteAccount));
    EXPECT_TRUE(user.CanPerform(UserAction::kDeleteOwnMessage));

    EXPECT_FALSE(user.CanPerform(UserAction::kModifyTag));
    EXPECT_FALSE(user.CanPerform(UserAction::kDeleteAnyMessage));
    EXPECT_FALSE(user.CanPerform(UserAction::kBanUser));
    EXPECT_FALSE(user.CanPerform(UserAction::kManageRoles));
  }
} // namespace user::tests