#include "boost_rest_client.h"

#include "Networking/http_exceptions.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;

using tcp = net::ip::tcp;


namespace {
  constexpr int kHttp1Dot1VersionIdf = 11;

  constexpr std::string_view kClientNameVersion = "CppChatSystemClient/1.0";

  constexpr std::string_view kJsonDataIdf = "application/json";
}

HttpResponse BoostRestClient::Get(std::string_view endpoint) {
  return SendRequest(http::verb::get, endpoint);
}

HttpResponse BoostRestClient::Post(std::string_view endpoint) {
  return SendRequest(http::verb::post, endpoint);
}

HttpResponse BoostRestClient::Put(std::string_view endpoint) {
  return SendRequest(http::verb::put, endpoint);
}

HttpResponse BoostRestClient::Delete(std::string_view endpoint) {
  return SendRequest(http::verb::delete_, endpoint);
}

void BoostRestClient::SetSNIHostName(beast::ssl_stream<beast::tcp_stream>& sslStream) const {
  if (!SSL_set_tlsext_host_name(sslStream.native_handle(), host_.c_str())) {

    beast::error_code ec{ static_cast<int>(::ERR_get_error()), net::error::get_ssl_category() };

    throw beast::system_error{ ec };
  }
}

void BoostRestClient::VerifyServerCertificate(ssl::context& sslCtx) {
  sslCtx.set_default_verify_paths();
  sslCtx.set_verify_mode(ssl::verify_peer);
}

void BoostRestClient::LoadAndAttachSessionToken(http::request<http::string_body>& req) const {
  auto identity = identity_repo_obs_->Load();

  if (identity.has_value() && !identity->session_token.empty()) {
    req.set(http::field::authorization, "Bearer " + identity->session_token);
  }
}

void BoostRestClient::CloseSSLStream(beast::ssl_stream<beast::tcp_stream>& sslStream) {
  beast::error_code ec;

  ec = sslStream.shutdown(ec);

  if (ec == net::error::eof || ec == ssl::error::stream_truncated) {
    ec = {};
  }

  if (ec) {
    throw beast::system_error{ ec };
  }
}
void BoostRestClient::ThrowOnErrorStatusCode(const unsigned statusCode, http::response<http::string_body>& res) {
  switch (statusCode) {
    case UnauthorizedException::StatusCode():
      throw UnauthorizedException();

    case NotFoundException::StatusCode():
      throw NotFoundException(res.body());

    case ValidationException::StatusCode():
      throw ValidationException(res.body());

    case ServerException::StatusCode():
      throw ServerException(res.body());

    default:;
      if (statusCode > ServerException::StatusCode()) {
        throw ServerException(res.body());
      }
  }
}

HttpResponse BoostRestClient::ParseAndReturnOkResponse(const unsigned statusCode, http::response<http::string_body>& res) {
  HttpResponse out_response;

  out_response.status_code = statusCode;

  if (!res.body().empty()) {
    out_response.body = nlohmann::json::parse(res.body());
  }
  else {
    out_response.body = nlohmann::json({});
  }

  return out_response;
}

void BoostRestClient::ResolveDNSAndConnect(ssl::context& sslCtx, tcp::resolver& resolver, beast::ssl_stream<beast::tcp_stream>& sslStream) const {
  auto const results = resolver.resolve(host_, port_);

  beast::get_lowest_layer(sslStream).connect(results);
}

HttpResponse BoostRestClient::SendRequest(http::verb method, std::string_view endpoint, const nlohmann::json* body) const {
  try {
    net::io_context ioc;
    ssl::context ssl_ctx(ssl::context::tlsv12_client);

    VerifyServerCertificate(ssl_ctx);

    tcp::resolver resolver(ioc);
    beast::ssl_stream<beast::tcp_stream> ssl_stream(ioc, ssl_ctx);

    SetSNIHostName(ssl_stream);

    ResolveDNSAndConnect(ssl_ctx, resolver, ssl_stream);

    ssl_stream.handshake(ssl::stream_base::client);

    http::request<http::string_body> req{ method, endpoint, kHttp1Dot1VersionIdf };

    req.set(http::field::host, host_);
    req.set(http::field::user_agent, kClientNameVersion);

    LoadAndAttachSessionToken(req);

    if (body != nullptr) {
      req.set(http::field::content_type, kJsonDataIdf);
      req.body() = body->dump();
      req.prepare_payload();
    }

    http::write(ssl_stream, req);

    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(ssl_stream, buffer, res);

    CloseSSLStream(ssl_stream);

    auto status_code = res.result_int();

    ThrowOnErrorStatusCode(status_code, res);

    return ParseAndReturnOkResponse(status_code, res);
  }
  catch (const nlohmann::json::parse_error& e) {
    throw std::runtime_error(std::format("Failed to parse JSON response from server: {}", e.what()));
  }
  catch (const std::exception& e) {
    throw std::runtime_error(std::format("Network request failed: {}", e.what()));
  }
}