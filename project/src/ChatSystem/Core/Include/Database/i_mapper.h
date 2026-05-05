#ifndef I_MAPPER_H
#define I_MAPPER_H

#include "i_row.h"

template <typename TDomain>
class IMapper {
public:

  [[nodiscard]] virtual TDomain Map(const IRow& row) = 0;
};

#endif // I_MAPPER_H
