#ifndef PQXX_RESULT_SET_H
#define PQXX_RESULT_SET_H


#include <functional>

#include <Database/i_row.h>
#include <Database/i_result_set.h>

#include <pqxx/pqxx>

#include <utility>

class PqxxResultSet : public IResultSet {
public:
  void ForEach(const std::function<void(const IRow&)>& action) const override;

  void First(const std::function<void(const IRow&)>& action) const override;

  [[nodiscard]] bool IsEmpty() const override;

  [[nodiscard]] std::size_t GetSize() const override;

  explicit PqxxResultSet(pqxx::result result);

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