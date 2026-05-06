#include "Infrastructure/Postgres/pqxx_row.h"

#include <Infrastructure/Postgres/pqxx_result_set.h>

void PqxxResultSet::ForEach(const std::function<void(const IRow&)>& action) const {
  for (auto&& row : result_) {
    action(PqxxRow{ row });
  }
}

void PqxxResultSet::First(const std::function<void(const IRow&)>& action) const {
  if (!result_.empty()) {
    action(PqxxRow{ result_[0] });
  }
}
