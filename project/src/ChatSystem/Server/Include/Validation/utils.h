#ifndef UTILS_H
#define UTILS_H

#include <ranges>

namespace validation {

  /**
   * @brief Safely computes the length or distance of a C++ range.
   * Optimizes for sized ranges (e.g., std::vector, std::string) using std::ranges::size,
   * while falling back to std::ranges::distance for unsized ranges.
   * 
   * @tparam TRange The type of the range.
   * @param rangeVal The range to measure.
   * @return std::size_t The number of elements in the range.
   */
  template<std::ranges::range TRange>
  constexpr std::size_t GetLength(const TRange& rangeVal) {
    if constexpr (std::ranges::sized_range<TRange>) {
      return std::ranges::size(rangeVal);
    }
    else {
      return std::ranges::distance(rangeVal);
    }
  }

}

#endif // UTILS_H