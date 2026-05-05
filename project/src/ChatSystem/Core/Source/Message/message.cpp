#include <Message/message.h>
#include <string_view>
#include <variant>

std::string_view Message::GetContent() const {
  return std::visit(
    [](auto&& payload) { return payload.GetContent(); },
    payload_
  );
}

std::string_view Message::GetTypeStr() const{
  return std::visit(
    [](auto&& payload) { return payload.TypeString(); },
    payload_
  );
}