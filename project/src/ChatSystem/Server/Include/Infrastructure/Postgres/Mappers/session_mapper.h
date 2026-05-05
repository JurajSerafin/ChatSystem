#ifndef SESSION_MAPPER_H
#define SESSION_MAPPER_H

#include <Database/i_mapper.h>
#include <Session/session.h>

class SessionMapper : public IMapper<Session> {
public:
  [[nodiscard]] Session Map(const IRow& row) override;
};

#endif // SESSION_MAPPER_H