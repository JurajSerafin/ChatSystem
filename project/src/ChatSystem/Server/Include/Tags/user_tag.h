#ifndef TAG_H
#define TAG_H

#include <algorithm>
#include <cctype>
#include <format>
#include <stdexcept>
#include <string>


namespace tags {
  /**
   * @brief Value Object representing a unique User Tag.
   *
   * A UserTag is an immutable identifier typically combining a login string
   * with a numeric suffix separated by a specific character (e.g., "login#1234").
   */
  class UserTag {
  public:
    /**
     * @brief Constructs and validates a new Tag.
     * @param value The raw string value to validate and encapsulate.
     * @throws std::invalid_argument if the string does not match the tag format.
     */
    explicit UserTag(std::string value) : value_{ std::move(value) } {
      Validate(value_);
    }

    /// @return The underlying string representation of the tag.
    [[nodiscard]] const std::string& ToString() const { return value_; }

    /// @brief Equality operator to compare two tags.
    bool operator==(const UserTag& other) const noexcept {
      return value_ == other.value_;
    }

    /**
     * @brief Checks if the tag instance holds a valid state.
     * @return true if valid.
     */
    bool IsValid();

  private:
    /// The character used to separate the login from the numeric suffix.
    constexpr static char kLoginDigitsSeparator = '#';

    /// The minimum length required for the login portion of the tag.
    constexpr static int kminLoginLen = 1;

    /// The exact number of digits required in the suffix.
    constexpr static int kSuffixLen = 4;

    /**
     * @brief Orchestrates the full validation of the tag string.
     * @param tagValue The string to validate.
     */
    static void Validate(const std::string& tagValue) {
      ValidateNonEmpty(tagValue);

      const std::size_t separator = tagValue.find(kLoginDigitsSeparator);

      ValidateSeparatorPresence(separator);
      ValidateLoginPartNonEmpty(separator);
      ValidateSuffix(separator, tagValue);
    }

    static void ValidateNonEmpty(const std::string& tagValue) {
      if (tagValue.empty()) {
        throw std::invalid_argument{ "Tag cannot be empty" };
      }
    }

    static void ValidateSeparatorPresence(const size_t separator) {
      if (separator == std::string::npos) {
        throw std::invalid_argument{ std::format("Tag must contain '{}'", kLoginDigitsSeparator) };
      }
    }

    static void ValidateLoginPartNonEmpty(const size_t separator) {
      if (separator < kminLoginLen) {
        throw std::invalid_argument{ "Tag login part cannot be empty" };
      }
    }

    static void ValidateSuffix(const size_t separator, const std::string& tagValue) {
      const auto suffix = tagValue.substr(separator + 1);

      if (suffix.size() != kSuffixLen || !std::ranges::all_of(suffix, ::isdigit)) {
        throw std::invalid_argument{ std::format("Tag suffix must be exactly {} digits long", kSuffixLen) };
      }
    }

    std::string value_;
  };
} // namespace tags


#endif // TAG_H