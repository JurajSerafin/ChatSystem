#ifndef CHAT_MAPPER_H
#define CHAT_MAPPER_H

#include <Database/i_mapper.h>
#include <Chat/chat.h>

class ChatMapper : public IMapper<Chat> {
public:
  [[nodiscard]] Chat Map(const IRow& row) override;
};

#endif // CHAT_MAPPER_H
