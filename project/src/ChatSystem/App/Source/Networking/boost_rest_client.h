#ifndef BOOST_REST_CLIENT_H
#define BOOST_REST_CLIENT_H

#include "Networking/i_rest_client.h"
#include "Repositories/i_local_identity_repository.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;

using tcp = net::ip::tcp;

class BoostRestClient : public IRestClient {
public:
  explicit BoostRestClient(ILocalIdentityRepository* identityRepoObs);

  HttpResponse Get(std::string_view endpoint) override;

  HttpResponse Post(std::string_view endpoint) override;

  HttpResponse Put(std::string_view endpoint) override;

  HttpResponse Delete(std::string_view endpoint) override;

  HttpResponse Post(std::string_view endpoint, const nlohmann::json& body) override;

  HttpResponse Put(std::string_view endpoint, const nlohmann::json& body) override;

private:
  HttpResponse SendRequest(http::verb method, std::string_view endpoint, const nlohmann::json* body = nullptr) const;

  void SetSNIHostName(beast::ssl_stream<beast::tcp_stream>& sslStream) const;

  static void VerifyServerCertificate(ssl::context& sslCtx);

  void LoadAndAttachSessionToken(http::request<http::string_body>& req) const;

  static void CloseSSLStream(beast::ssl_stream<beast::tcp_stream>& sslStream);

  static void ThrowOnErrorStatusCode(const unsigned statusCode, http::response<http::string_body>& res);

  static HttpResponse ParseAndReturnOkResponse(const unsigned statusCode, http::response<http::string_body>& res);

  void ResolveDNSAndConnect(
    ssl::context& sslCtx,
    boost::asio::ip::tcp::resolver& resolver,
    beast::ssl_stream<beast::tcp_stream>& sslStream
  ) const;


  ILocalIdentityRepository* identity_repo_obs_;

  std::string host_;
  std::string port_;
};

inline BoostRestClient::BoostRestClient(ILocalIdentityRepository* identityRepoObs) : identity_repo_obs_(identityRepoObs) {}

#endif // BOOST_REST_CLIENT_H