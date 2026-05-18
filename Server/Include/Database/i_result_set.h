#ifndef I_RESULT_SET_H
#define I_RESULT_SET_H

#include <functional>

#include "i_row.h"

/**
 * @brief Interface representing the dataset returned by a database query.
 * * Provides functional iterators to safely extract data from the returned rows.
 */
class IResultSet {
public:
  virtual ~IResultSet() = default;

  /**
   * @brief Iterates sequentially over every row in the result set.
   * @param action A lambda/callback executed for each row.
   */
  virtual void ForEach(const std::function<void(const IRow&)>& action) const = 0;

  /**
   * @brief Extracts data only from the first row of the result set.
   * @param action A lambda/callback executed on the first row, if it exists.
   */
  virtual void First(const std::function<void(const IRow&)>& action) const = 0;

  /**
   * @brief Checks if the query returned zero rows.
   * @return True if empty, false otherwise.
   */
  [[nodiscard]] virtual bool IsEmpty() const = 0;

  /**
   * @brief Gets the total number of rows returned by the query.
   * @return The exact row count.
   */
  [[nodiscard]] virtual std::size_t GetSize() const = 0;
};

#endif // I_RESULT_SET_H