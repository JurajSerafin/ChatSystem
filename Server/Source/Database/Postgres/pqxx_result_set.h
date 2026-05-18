#ifndef PQXX_RESULT_SET_H
#define PQXX_RESULT_SET_H

#include <functional>

#include <Database/i_row.h>
#include <Database/i_result_set.h>

#include <pqxx/pqxx>

#include <utility>

/**
 * @brief Concrete implementation of a result set wrapping a `pqxx::result`.
 * * Acts as an immutable, memory-safe container for the raw data returned by
 * a PostgreSQL query, allowing functional iteration and extraction of records.
 */
class PqxxResultSet : public IResultSet {
public:
  /**
   * @brief Takes ownership of a raw libpqxx result object.
   */
  explicit PqxxResultSet(pqxx::result result);

  void ForEach(const std::function<void(const IRow&)>& action) const override;
  void First(const std::function<void(const IRow&)>& action) const override;
  [[nodiscard]] bool IsEmpty() const override;
  [[nodiscard]] std::size_t GetSize() const override;

private:
  pqxx::result result_;
};

inline bool PqxxResultSet::IsEmpty() const {
  return result_.empty();
}

inline std::size_t PqxxResultSet::GetSize() const {
  return result_.size();
}

inline PqxxResultSet::PqxxResultSet(pqxx::result result) : result_(std::move(result)) {}

#endif // PQXX_RESULT_SET_H