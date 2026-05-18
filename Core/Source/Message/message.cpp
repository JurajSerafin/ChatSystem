#include <Message/message.h>
#include <string_view>
#include <variant>

std::string_view Message::GetCiphertext() const {
  return ciphertext_;
}

std::string Message::GetTypeStr() const{
  return std::visit(
    [](auto&& payload) { return std::string(payload.TypeString()); },
    type_
  );
}