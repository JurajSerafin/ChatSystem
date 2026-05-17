#include <iostream>
#include <thread>
#include <memory>
#include <stdexcept>
#include <format>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

#include "Networking/router.h"
#include "Networking/Controllers/auth_controller.h"
#include "Networking/Controllers/user_controller.h"
#include "Networking/Controllers/chat_controller.h"
#include "Networking/Controllers/message_controller.h"

#include "Services/Implementation/auth_service.h"
#include "Services/Implementation/user_service.h"
#include "Services/Implementation/chat_service.h"
#include "Services/Implementation/message_service.h"
#include "Services/Implementation/notification_service.h"
#include "Infrastructure/OpenSSL/open_ssl_server_encryption_service.h"
#include "Infrastructure/Postgres/pqxx_connection_pool.h"
#include "Infrastructure/Postgres/Repositories/pqxx_user_repository.h"
#include "Infrastructure/Postgres/Repositories/pqxx_session_repository.h"
#include "Infrastructure/Postgres/Repositories/pqxx_chat_repository.h"
#include "Infrastructure/Postgres/Repositories/pqxx_message_repository.h"

#include "Session/session_validator.h"
#include "User/user_validator.h"


namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

namespace {
  constexpr auto kIpAddress = "0.0.0.0";

  constexpr auto kPort = 8080;

  constexpr auto kLoadBalancingFactor = 4;


  void HandleSession(tcp::socket socket, Router& router) {
    try {
        beast::flat_buffer buffer;
        beast::error_code error_code;

        while (true) {
        http::request<http::string_body> request;
        http::read(socket, buffer, request, error_code);

        if (error_code == http::error::end_of_stream) {
          break;
        }

        if (error_code) {
          std::cerr << "Read Error: " << error_code.message() << "\n";
          break;
        }

        auto response = router.RouteRequest(request);

        http::write(socket, response, error_code);

        if (error_code) {
          std::cerr << "Write Error: " << error_code.message() << "\n";
          break;
        }

        if (response.need_eof()) {
          break;
        }
      }

      auto shutdown = socket.shutdown(tcp::socket::shutdown_send, error_code);

      if (error_code) {
        std::cerr << "Socket Shutdown Error: " << shutdown.message() << "\n";
      }

    }
    catch (std::exception& e) {
      std::cerr << "Session Exception: " << e.what() << "\n";
    }

  }

  void LoadtEnvironment() {
#ifdef ENV_FILE_PATH
    std::ifstream file(ENV_FILE_PATH);

    constexpr char kCommentLineStartCHar = '#';
    constexpr char kKeyValueDelim = '=';

    if (!file.is_open()) {
      return;
    }

    std::string line;

    while (std::getline(file, line)) {

      if (line.empty() || line[0] == kCommentLineStartCHar) {
        continue;
      }

      auto delimiter_pos = line.find(kKeyValueDelim);

      if (delimiter_pos != std::string::npos) {
        std::string key = line.substr(0, delimiter_pos);

        std::string value = line.substr(delimiter_pos + 1);

        if (!value.empty() && value.back() == '\r') {
          value.pop_back();
        }
#ifdef _WIN32
        _putenv_s(key.c_str(), value.c_str());
#else // _WIN32
        setenv(key.c_str(), value.c_str(), 1);
#endif // _WIN32 else
      }
    }
#endif // ENV_FILE_PATH
  }
} // namespace


int main() {

  try {
    LoadtEnvironment();

    std::cout << "[1/4] Initializing Database Connection Pool...\n";

    PqxxConnectionPool connection_pool{};
    connection_pool.Init();

    std::cout << "[2/4] Wiring Repositories and Services...\n";

    PqxxUserRepository user_repo{ &connection_pool };
    PqxxSessionRepository session_repo{ &connection_pool };
    PqxxChatRepository chat_repo{ &connection_pool };
    PqxxMessageRepository msg_repo{ &connection_pool };

    const UserValidator user_validator{};
    const SessionValidator session_validator{};

    OpenSSLServerEncryptionService encryption_service{};
    AuthService auth_service{ user_repo, session_repo, encryption_service, user_validator, session_validator };
    UserService user_service{ &user_repo };
    ChatService chat_service{ &chat_repo, &user_repo };
    NotificationService notification_service{};
    MessageService msg_service{ &notification_service , &msg_repo, &chat_repo };

    std::cout << "[3/4] Registering HTTP Controllers...\n";

    AuthController auth_controller{ &auth_service, &user_service };
    UserController user_controller{ &auth_service, &user_service };
    ChatController chat_controller{ &auth_service, &chat_service };
    MessageController msg_controller{ &auth_service, &msg_service };

    Router router{};

    auth_controller.RegisterRoutes(router);
    user_controller.RegisterRoutes(router);
    chat_controller.RegisterRoutes(router);
    msg_controller.RegisterRoutes(router);

    std::cout << "[4/4] Starting Boost.Asio Network Server...\n";

    net::io_context ioc;

    tcp::acceptor acceptor(ioc, { net::ip::make_address(kIpAddress), kPort });

    std::cout << std::format("ChatSystem Server is up and running on port {}", kPort) << "\n";

    const auto thread_count = std::thread::hardware_concurrency() * kLoadBalancingFactor;

    boost::asio::thread_pool thread_pool (thread_count);

    while (true) {
      tcp::socket socket{ ioc };
      acceptor.accept(socket);

      boost::asio::post(thread_pool, [&router, skt = std::move(socket)] () mutable {
        HandleSession(std::move(skt), router);
      });
    }

  } catch (const std::exception& e) {
    std::cerr << "Server crashed with exception: " << e.what() << "\n";

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}