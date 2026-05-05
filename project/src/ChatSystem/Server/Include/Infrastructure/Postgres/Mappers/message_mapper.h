#ifndef SESSION_MAPPER_H
#define SESSION_MAPPER_H

#include <Database/i_mapper.h>
#include <Message/message.h>

class MessageMapper : public IMapper<Message> {
public:
  [[nodiscard]] Message Map(const IRow& row) override;
};

#endif // SESSION_MAPPER_H