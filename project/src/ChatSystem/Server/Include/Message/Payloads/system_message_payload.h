#ifndef SYSTEM_MESSAGE_PAYLOAD_H
#define SYSTEM_MESSAGE_PAYLOAD_H

#include <string>


class SystemMessagePayload {
private:
  std::string content_;

  constexpr static std::size_t kMaxContentLength = 10000;

public:
  explicit SystemMessagePayload(std::string content);

  [[nodiscard]] std::string_view GetContent() const;

  [[nodiscard]] static constexpr std::string_view TypeString();

  [[nodiscard]] bool IsValid() const noexcept;
};

inline SystemMessagePayload::SystemMessagePayload(std::string content) : content_(std::move(content)) {}

inline std::string_view SystemMessagePayload::GetContent() const {
  return content_;
}

inline bool SystemMessagePayload::IsValid() const noexcept {
  return !content_.empty() && content_.size() <= kMaxContentLength;
}

constexpr std::string_view SystemMessagePayload::TypeString() {
  return "SYSTEM";
}

#endif // SYSTEM_MESSAGE_PAYLOAD_H