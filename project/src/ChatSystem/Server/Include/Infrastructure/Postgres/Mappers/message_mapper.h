#ifndef SESSION_MAPPER_H
#define SESSION_MAPPER_H

#include <Message/message.h>

class IRow;

class MessageMapper {
public:
  [[nodiscard]] static Message Map(const IRow& row);
};

#endif // SESSION_MAPPER_H