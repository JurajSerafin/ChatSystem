#ifndef CHAT_MAPPER_H
#define CHAT_MAPPER_H

#include <Chat/chat.h>
#include <User/user_id.h>
#include <vector>

class IRow;

class ChatMapper {
public:
  [[nodiscard]] static Chat Map(const IRow& row, std::vector<UserId> participants);
};

#endif // CHAT_MAPPER_H