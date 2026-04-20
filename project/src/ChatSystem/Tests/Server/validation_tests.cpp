#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

#include <Validation/validation.h>

namespace validation::tests {

  // Core Mechanics Tests

  TEST(ValidationResultTest, DefaultStateIsOk) {
    // Arrange & Act
    const ValidationResult<3> result;

    // Assert
    EXPECT_TRUE(result.Ok());
    EXPECT_EQ(result.count, 0);
  }

  TEST(ValidationResultTest, AddErrorByValues) {
    // Arrange
    ValidationResult<2> result;

    // Act
    result.AddError("username", "Cannot be empty");

    // Assert
    EXPECT_FALSE(result.Ok());
    EXPECT_EQ(result.count, 1);
    EXPECT_EQ(result.errors[0].field, "username");
    EXPECT_EQ(result.errors[0].message, "Cannot be empty");
  }

  TEST(ValidationResultTest, RespectsCapacityLimits) {
    // Arrange
    ValidationResult<1> result;

    // Act
    result.AddError("f1", "m1");
    result.AddError("f2", "m2");

    // Assert
    EXPECT_EQ(result.count, 1);
    EXPECT_EQ(result.errors[0].field, "f1");
  }

  // Primitive Rule Tests

  TEST(IsNotNullRuleTest, ValidatesRawAndSmartPointers) {
    // Arrange
    auto rule = rules::IsNotNull;
    int value = 5;
    int* valid_ptr = &value;
    int* null_ptr = nullptr;
    auto smart_ptr = std::make_unique<int>(10);
    const std::unique_ptr<int> null_smart_ptr = nullptr;

    // Act
    const auto valid_result = rule(valid_ptr);
    const auto smart_result = rule(smart_ptr);
    const auto null_result = rule(null_ptr);
    const auto null_smart_result = rule(null_smart_ptr);

    // Assert
    EXPECT_TRUE(valid_result.Ok());
    EXPECT_TRUE(smart_result.Ok());
    EXPECT_FALSE(null_result.Ok());
    EXPECT_FALSE(null_smart_result.Ok());
    EXPECT_EQ(null_result.errors[0].message, "Must not be null");
  }

  struct MockValidatable {
    bool is_valid;

    [[nodiscard]] bool IsValid() const { 
      return is_valid;
    }
  };

  TEST(IsValidRuleTest, ValidatesHasIsValidConcept) {
    // Arrange
    constexpr auto rule = rules::IsValid;
    constexpr MockValidatable valid_obj{ .is_valid = true };
    constexpr MockValidatable invalid_obj{ .is_valid = false };

    // Act
    const auto valid_result = rule(valid_obj);
    const auto invalid_result = rule(invalid_obj);

    // Assert
    EXPECT_TRUE(valid_result.Ok());
    EXPECT_FALSE(invalid_result.Ok());
    EXPECT_EQ(invalid_result.errors[0].message, "Has to be valid");
  }

  TEST(LengthRulesTest, MaxLengthChecksCorrectly) {
    // Arrange
    constexpr auto rule = rules::MaxLength<5>;
    const std::string valid_str = "12345";
    const std::vector valid_vec = { 1, 2, 3 };
    const std::string invalid_str = "123456";

    // Act
    const auto valid_str_result = rule(valid_str);
    const auto valid_vec_result = rule(valid_vec);
    const auto invalid_str_result = rule(invalid_str);

    // Assert
    EXPECT_TRUE(valid_str_result.Ok());
    EXPECT_TRUE(valid_vec_result.Ok());
    EXPECT_FALSE(invalid_str_result.Ok());
    EXPECT_EQ(invalid_str_result.errors[0].message, "Is too long");
  }

  TEST(LengthRulesTest, MinLengthChecksCorrectly) {
    // Arrange
    constexpr auto rule = rules::MinLength<3>;
    const std::string valid_str = "123";
    const std::string invalid_str = "12";

    // Act
    const auto valid_result = rule(valid_str);
    const auto invalid_result = rule(invalid_str);

    // Assert
    EXPECT_TRUE(valid_result.Ok());
    EXPECT_FALSE(invalid_result.Ok());
    EXPECT_EQ(invalid_result.errors[0].message, "Is too short");
  }

  TEST(NotEmptyRuleTest, ValidatesRanges) {
    // Arrange
    constexpr auto rule = rules::NotEmpty;
    const std::string valid_str = "a";
    const std::string invalid_str;
    const std::vector<int> empty_vec;

    // Act
    auto valid_str_result = rule(valid_str);
    auto invalid_str_result = rule(invalid_str);
    auto empty_vec_result = rule(empty_vec);

    // Assert
    EXPECT_TRUE(valid_str_result.Ok());
    EXPECT_FALSE(invalid_str_result.Ok());
    EXPECT_FALSE(empty_vec_result.Ok());
  }

  TEST(TimePointRuleTest, ValidatesConcreteTime) {
    // Arrange
    constexpr auto rule = rules::TimePointMustBeSet;
    constexpr auto empty_time = std::chrono::system_clock::time_point{};
    const auto valid_time = std::chrono::system_clock::now();

    // Act
    const auto empty_result = rule(empty_time);
    const auto valid_result = rule(valid_time);

    // Assert
    EXPECT_FALSE(empty_result.Ok());
    EXPECT_TRUE(valid_result.Ok());
  }

  // Composition and Binding Tests

  TEST(AndRuleTest, CombinesCapacitiesAndErrors) {
    // Arrange
    auto combined_rule = rules::NotEmpty && rules::MinLength<5>;
    const std::string valid_str = "hello world";
    const std::string empty_str;

    static_assert(decltype(combined_rule)::capacity == 2);

    // Act
    auto valid_result = combined_rule(valid_str);
    auto invalid_result = combined_rule(empty_str);

    // Assert
    EXPECT_TRUE(valid_result.Ok());
    EXPECT_FALSE(invalid_result.Ok());
    EXPECT_EQ(invalid_result.count, 2);
    EXPECT_EQ(invalid_result.errors[0].message, "Must not be empty");
    EXPECT_EQ(invalid_result.errors[1].message, "Is too short");
  }

  struct DummyUser {
    std::string username;
  };

  TEST(BoundRuleTest, InjectsFieldNameIntoErrors) {
    // Arrange
    rules::Field<DummyUser, std::string> username_field{ &DummyUser::username, "username" };
    auto username_rule = username_field | (rules::NotEmpty && rules::MinLength<3>);
    DummyUser valid_user{ "Bob" };
    DummyUser invalid_user{ "" };

    // Act
    auto valid_result = username_rule(valid_user);
    auto invalid_result = username_rule(invalid_user);

    // Assert
    EXPECT_TRUE(valid_result.Ok());
    EXPECT_FALSE(invalid_result.Ok());
    EXPECT_EQ(invalid_result.count, 2);
    EXPECT_EQ(invalid_result.errors[0].field, "username");
    EXPECT_EQ(invalid_result.errors[0].message, "Must not be empty");
    EXPECT_EQ(invalid_result.errors[1].field, "username");
    EXPECT_EQ(invalid_result.errors[1].message, "Is too short");
  }

}