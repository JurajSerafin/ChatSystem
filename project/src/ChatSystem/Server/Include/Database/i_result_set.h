#ifndef I_RESULT_SET_H
#define I_RESULT_SET_H

#include <functional>
#include <optional>
#include <cstddef>

#include "i_row.h"

class IResultSet {
public:

  virtual void ForEach(const std::function<void(const IRow&)>& action) const = 0;

  virtual void First(const std::function<void(const IRow&)>& action) const = 0;

  [[nodiscard]] virtual bool IsEmpty() const = 0;

  [[nodiscard]] virtual std::size_t GetSize() const = 0;

  virtual ~IResultSet() = default;

};


#endif // I_RESULT_SET_H