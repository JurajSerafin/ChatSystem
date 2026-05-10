#ifndef SYSTEM_MESSAGE_TYPE_H
#define SYSTEM_MESSAGE_TYPE_H

#include <string_view>

class SystemMessageType {
private:
  constexpr static std::size_t kMaxContentLength = 255;

  constexpr static std::string_view kTypeString = "SYSTEM";

public:
  [[nodiscard]] constexpr static std::size_t GetMaxLength();

  [[nodiscard]] static constexpr std::string_view TypeString();

};

constexpr std::size_t SystemMessageType::GetMaxLength() {
  return kMaxContentLength;
}

constexpr std::string_view SystemMessageType::TypeString() {
  return kTypeString;
}

#endif // SYSTEM_MESSAGE_TYPE_H