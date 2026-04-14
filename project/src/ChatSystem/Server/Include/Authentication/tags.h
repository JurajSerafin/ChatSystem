#ifndef TAG_GENERATOR_H
#define TAG_GENERATOR_H

#include <string>
#include <random>
#include <format>

#include <Tags/tag.h>

namespace tags {

  /**
   * @brief Generates a unique user tag from a login.
   *
   * The generated tag is composed of:
   * - the provided login string
   * - a separator character
   * - a random 4-digit number in the range
   *
   * Example format:
   * @code
   * login#1234
   * @endcode
   *
   * A thread-local random number generator is used to ensure
   * thread safety while maintaining good performance.
   *
   * @param login Base login used to generate the tag.
   * @return Generated Tag containing login and random suffix.
   */
  inline Tag GenerateFromLogin(const std::string& login) {

    thread_local std::mt19937 twister{ std::random_device{}() };

    /// Separator between login and numeric suffix.
    constexpr char kLoginTagSeparator = '#';

    /// Minimum value for random suffix.
    constexpr int kMin = 1000;

    /// Maximum value for random suffix.
    constexpr int kMax = 9999;

    std::uniform_int_distribution<int> number_distribution{ kMin, kMax };

    return Tag{
        std::format("{}{}{}",
            login,
            kLoginTagSeparator,
            number_distribution(twister)
        )
    };
  }

} // namespace tags

#endif // TAG_GENERATOR_H