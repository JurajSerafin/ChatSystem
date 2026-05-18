#ifndef TEXT_MESSAGE_TYPE_H
#define TEXT_MESSAGE_TYPE_H

#include <string_view>

class TextMessageType {
private:
  constexpr static std::size_t kMaxContentLength = 1000;

  constexpr static std::string_view kTypeString = "TEXT";

public:
  [[nodiscard]] constexpr static std::size_t GetMaxLength();

  [[nodiscard]] constexpr static std::string_view TypeString();
};

constexpr std::size_t TextMessageType::GetMaxLength() {
  return kMaxContentLength;
}

constexpr std::string_view TextMessageType::TypeString() {
  return kTypeString;
}

#endif // TEXT_MESSAGE_TYPE_H