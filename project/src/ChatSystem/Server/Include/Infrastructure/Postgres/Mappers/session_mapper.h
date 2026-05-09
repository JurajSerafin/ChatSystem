#ifndef SESSION_MAPPER_H
#define SESSION_MAPPER_H

#include <Session/session.h>

class IRow;

class SessionMapper {
public:
  [[nodiscard]] static Session Map(const IRow& row);
};

#endif // SESSION_MAPPER_H