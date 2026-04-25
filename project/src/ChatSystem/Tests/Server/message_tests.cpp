#include <Message/message.h>
#include <Message/message_params.h>
#include <Message/message_validator.h>
#include <chrono>
#include <stdexcept>

#include <gtest/gtest.h>

namespace message::tests {

  // Helper to generate a baseline valid params object
  static MessageParams CreateValidMessageParams() {
    return MessageParams{
      .id = MessageId::Generate(),
      .chat_id = ChatId::Generate(),
      .sender_id = UserId::Generate(),
      .content = "Hello, world!",
      .type = MessageType::kText,
      .created_at = std::chrono::system_clock::now()
    };
  }

  // ---------------------------------------------------------
  // Message Creation & Validation
  // ---------------------------------------------------------

  TEST(MessageTest, CreateSucceedsWithValidParameters) {
    // Arrange
    MessageValidator validator;
    MessageParams params = CreateValidMessageParams();
    const auto expected_id = params.id;
    const auto expected_chat_id = params.chat_id;
    const auto expected_sender_id = params.sender_id;

    // Act
    Message message = Message::Create(std::move(params), validator);

    // Assert
    EXPECT_EQ(message.GetId(), expected_id);
    EXPECT_EQ(message.GetChatId(), expected_chat_id);
    EXPECT_EQ(message.GetSenderId(), expected_sender_id);
    EXPECT_EQ(message.GetContent(), "Hello, world!");
    EXPECT_EQ(message.GetType(), MessageType::kText);
  }

  TEST(MessageTest, CreateFailsWhenContentIsEmpty) {
    // Arrange
    MessageValidator validator;
    MessageParams params = CreateValidMessageParams();

    params.content = "";

    // Act & Assert
    EXPECT_THROW({
      Message::Create(std::move(params), validator);
      }, std::invalid_argument
    );
  }

  TEST(MessageTest, CreateFailsWhenContentExceedsMaxLength) {
    // Arrange
    MessageValidator validator;
    MessageParams params = CreateValidMessageParams();

    params.content = std::string(10001, 'a'); // kMaxContentLength = 10000


    // Act & Assert
    EXPECT_THROW({
      Message::Create(std::move(params), validator);
      }, std::invalid_argument
    );
  }

} // namespace message::tests