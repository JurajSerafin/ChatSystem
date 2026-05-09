#ifndef USER_MAPPER_H
#define USER_MAPPER_H

#include <User/user.h>

class IRow;

class UserMapper {
public:
  [[nodiscard]] static User Map(const IRow& row);
};

#endif // USER_MAPPER_H