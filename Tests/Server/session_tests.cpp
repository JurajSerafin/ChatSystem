#include <gtest/gtest.h>
#include <Session/session.h>
#include <Session/session_validator.h>
#include <Session/session_params.h>
#include <Session/session_id.h>
#include <chrono>

namespace session::tests {

  // Helper to generate a baseline valid params object
  static SessionParams CreateValidSessionParams() {
    const auto now = std::chrono::system_clock::now();
    return SessionParams{
      .id = SessionId::Generate(),
      .user_id = UserId::Generate(),
      .token = std::string(32, 'x'), // kMinTokenLength = 32
      .expires_at = now + std::chrono::hours(1),
      .created_at = now
    };
  }

  // ---------------------------------------------------------
  // Session Creation & Validation
  // ---------------------------------------------------------

  TEST(SessionTest, CreateSucceedsWithValidParameters) {
    // Arrange
    SessionValidator validator;
    SessionParams params = CreateValidSessionParams();

    // Act
    Session session = Session::Create(std::move(params), validator);

    // Assert
    EXPECT_FALSE(session.IsExpired());
    EXPECT_EQ(session.GetToken().length(), 32);
  }

  TEST(SessionTest, CreateFailsWhenTokenIsTooShort) {
    // Arrange
    SessionValidator validator;
    SessionParams params = CreateValidSessionParams();

    params.token = "tk";

    // Act & Assert
    EXPECT_THROW({
      Session::Create(std::move(params), validator);
      }, std::invalid_argument
    );
  }

  TEST(SessionTest, CreateFailsWhenCreatedAtIsAfterExpiresAt) {
    // Arrange
    SessionValidator validator;
    SessionParams params = CreateValidSessionParams();

    params.created_at = params.expires_at + std::chrono::minutes(5);

    // Act & Assert
    EXPECT_THROW({
      Session::Create(std::move(params), validator);
      }, std::invalid_argument
    );
  }

  // ---------------------------------------------------------
  // Session Behavior
  // ---------------------------------------------------------

  TEST(SessionTest, IsExpiredReturnsTrueWhenTimeIsPastExpiration) {
    // Arrange
    SessionValidator validator;
    SessionParams params = CreateValidSessionParams();

    params.expires_at = std::chrono::system_clock::now() - std::chrono::hours(1);

    params.created_at = params.expires_at - std::chrono::hours(1);

    Session session = Session::Create(std::move(params), validator);

    // Act
    const bool is_expired = session.IsExpired();

    // Assert
    EXPECT_TRUE(is_expired);
  }

  TEST(SessionTest, RefreshExtendsExpirationTime) {
    // Arrange
    SessionValidator validator;
    Session session = Session::Create(CreateValidSessionParams(), validator);
    const auto original_expiration = session.ExpiresAt();
    constexpr auto kExtension = std::chrono::hours(24);

    // Act
    session.Refresh(kExtension);

    // Assert
    EXPECT_GT(session.ExpiresAt(), original_expiration);
    EXPECT_FALSE(session.IsExpired());
  }
} // namespace session::tests