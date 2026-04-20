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

  // IsLessThanInclusive Rule Tests

  TEST(IsLessThanInclusiveRuleTest, ValidatesIntegerBoundsCorrectly) {
    // Arrange
    constexpr int kThreshold = 10;
    constexpr auto rule = rules::IsLessThanInclusive(kThreshold);

    constexpr int valid_less = 5;
    constexpr int valid_equal = 10;
    constexpr int invalid_greater = 15;

    // Act
    const auto less_result = rule(valid_less);
    const auto equal_result = rule(valid_equal);
    const auto greater_result = rule(invalid_greater);

    // Assert
    EXPECT_TRUE(less_result.Ok());
    EXPECT_TRUE(equal_result.Ok());

    EXPECT_FALSE(greater_result.Ok());
    EXPECT_EQ(greater_result.count, 1);
    EXPECT_EQ(greater_result.errors[0].message, "Must be less than or equal to the threshold");
  }

  TEST(IsLessThanInclusiveRuleTest, ValidatesTimePointsCorrectly) {
    // Arrange
    const auto threshold_time = std::chrono::system_clock::now();
    const auto rule = rules::IsLessThanInclusive(threshold_time);

    const auto past_time = threshold_time - std::chrono::hours(1);
    const auto future_time = threshold_time + std::chrono::hours(1);

    // Act
    const auto past_result = rule(past_time);
    const auto exact_result = rule(threshold_time);
    const auto future_result = rule(future_time);

    // Assert
    EXPECT_TRUE(past_result.Ok());
    EXPECT_TRUE(exact_result.Ok());

    EXPECT_FALSE(future_result.Ok());
    EXPECT_EQ(future_result.count, 1);
    EXPECT_EQ(future_result.errors[0].message, "Must be less than or equal to the threshold");
  }

  // BIND_FIELD Macro & Field Binding Tests

  struct MacroTestUser {
    int age;
    std::string username;
  };

  TEST(BindFieldMacroTest, DeducesTypesAndStoresNameCorrectly) {
    // Arrange & Act
    auto age_field = BIND_FIELD(MacroTestUser, age);
    auto username_field = BIND_FIELD(MacroTestUser, username);

    // Assert
    static_assert(std::is_same_v<decltype(age_field.member), int MacroTestUser::*>);
    static_assert(std::is_same_v<decltype(username_field.member), std::string MacroTestUser::*>);

    // Assert (Runtime Stringification)
    EXPECT_EQ(age_field.name, "age");
    EXPECT_EQ(username_field.name, "username");
  }

  TEST(BindFieldMacroTest, IntegratesWithBoundRuleExecution) {
    // Arrange
    auto age_rule = BIND_FIELD(MacroTestUser, age) | rules::IsLessThanInclusive(100);

    const MacroTestUser valid_user{ .age = 25, .username = "Bob" };
    const MacroTestUser invalid_user{ .age = 150, .username = "Alice" };

    // Act
    const auto valid_result = age_rule(valid_user);
    const auto invalid_result = age_rule(invalid_user);

    // Assert
    EXPECT_TRUE(valid_result.Ok());

    EXPECT_FALSE(invalid_result.Ok());
    EXPECT_EQ(invalid_result.count, 1);

    EXPECT_EQ(invalid_result.errors[0].field, "age");
  }

  // BIND_FIELD Macro & Composition (&&) Tests

  TEST(BindFieldCompositionTest, ChainsMultipleBoundRulesLikeAValidator) {
    // Arrange
    auto validator_tree =
      (BIND_FIELD(MacroTestUser, username) | (rules::NotEmpty && rules::MinLength<3>)) &&
      (BIND_FIELD(MacroTestUser, age) | rules::IsLessThanInclusive(130));

    MacroTestUser valid_dto{ .username = "alice", .age = 30 };

    // Fails MinLength and IsLessThanInclusive
    MacroTestUser partially_invalid_dto{ .username = "a", .age = 150 };

    // Fails NotEmpty, MinLength and IsLessThanInclusive
    MacroTestUser totally_invalid_dto{ .username = "", .age = 999 };

    // Act
    const auto valid_result = validator_tree(valid_dto);
    const auto partial_result = validator_tree(partially_invalid_dto);
    const auto total_result = validator_tree(totally_invalid_dto);

    // Assert

    // NotEmpty(1) + MinLength(1) + IsLessThanInclusive(1) = 3
    static_assert(decltype(validator_tree)::capacity == 3);

    EXPECT_TRUE(valid_result.Ok());
    EXPECT_EQ(valid_result.count, 0);

    EXPECT_FALSE(partial_result.Ok());
    EXPECT_EQ(partial_result.count, 2);

    EXPECT_EQ(partial_result.errors[0].field, "username_");
    EXPECT_EQ(partial_result.errors[0].message, "Is too short");

    EXPECT_EQ(partial_result.errors[1].field, "age_");
    EXPECT_EQ(partial_result.errors[1].message, "Must be less than or equal to the threshold");

    EXPECT_FALSE(total_result.Ok());
    EXPECT_EQ(total_result.count, 3);

    EXPECT_EQ(total_result.errors[0].field, "username_");
    EXPECT_EQ(total_result.errors[0].message, "Must not be empty");

    EXPECT_EQ(total_result.errors[1].field, "username_");
    EXPECT_EQ(total_result.errors[1].message, "Is too short");

    EXPECT_EQ(total_result.errors[2].field, "age_");
    EXPECT_EQ(total_result.errors[2].message, "Must be less than or equal to the threshold");
  }
}