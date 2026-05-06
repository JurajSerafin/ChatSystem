#ifndef USER_MAPPER_H
#define USER_MAPPER_H

#include <Database/i_mapper.h>
#include <User/regular_user_role.h>
#include <User/user.h>
#include <tuple>

class UserMapper : public IMapper<User> {
public:
  [[nodiscard]] User Map(const IRow& row) override;
};

#endif // USER_MAPPER_H