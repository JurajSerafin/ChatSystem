#ifndef TEXT_MESSAGE_PAYLOAD_H
#define TEXT_MESSAGE_PAYLOAD_H

#include <string>
#include <string_view>

class TextMessagePayload {
private:
  std::string content_;

  constexpr static std::size_t kMaxContentLength = 10000;

public:

  explicit TextMessagePayload(std::string content);

  [[nodiscard]] std::string_view GetContent() const;

  [[nodiscard]] static constexpr std::string_view TypeString();

  [[nodiscard]] bool IsValid() const noexcept;

};

inline TextMessagePayload::TextMessagePayload(std::string content) : content_(std::move(content)) {}

inline std::string_view TextMessagePayload::GetContent() const {
  return content_;
}

constexpr std::string_view TextMessagePayload::TypeString() {
  return "SYSTEM";
}

inline bool TextMessagePayload::IsValid() const noexcept {
  return !content_.empty() && content_.size() <= kMaxContentLength;
}

#endif // TEXT_MESSAGE_PAYLOAD_H