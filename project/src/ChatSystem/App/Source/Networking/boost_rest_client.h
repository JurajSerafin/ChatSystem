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

/**
 * @brief Concrete implementation of the REST client using Boost.Beast and Boost.Asio.
 * * This class handles the low-level execution of HTTP/HTTPS requests. It manages
 * secure TLS/SSL connections, DNS resolution, and automatically attaches the active
 * user's session token to outbound requests via the injected identity repository.
 */
class BoostRestClient : public IRestClient {
public:
  /**
   * @brief Constructs the Boost-powered REST client.
   * @param identityRepoObs Observer pointer to the local identity repository.
   */
  explicit BoostRestClient(ILocalIdentityRepository* identityRepoObs);

  // IRestClient Interface Implementation

  HttpResponse Get(std::string_view endpoint) override;

  HttpResponse Post(std::string_view endpoint) override;

  HttpResponse Put(std::string_view endpoint) override;

  HttpResponse Delete(std::string_view endpoint) override;

  HttpResponse Post(std::string_view endpoint, const nlohmann::json& body) override;

  HttpResponse Put(std::string_view endpoint, const nlohmann::json& body) override;

private:
  /**
   * @brief Core networking engine that dispatches all HTTP requests.
   * * Orchestrates the entire networking pipeline: DNS resolution, SSL handshake,
   * payload packaging, HTTP transmission, receiving, and connection teardown.
   * * @param method The HTTP verb (GET, POST, PUT, DELETE).
   * @param endpoint The target URL path routing (e.g., "/api/v1/users").
   * @param body Optional JSON payload. If nullptr, no body is attached to the request.
   * @return The fully parsed standard HttpResponse.
   */
  HttpResponse SendRequest(http::verb method, std::string_view endpoint, const nlohmann::json* body = nullptr) const;

  /**
   * @brief Sets the Server Name Indication (SNI) for the TLS connection.
   * @param sslStream The active Boost SSL stream to configure.
   */
  void SetSNIHostName(beast::ssl_stream<beast::tcp_stream>& sslStream) const;

  /**
   * @brief Configures the SSL context to verify the remote server's certificate.
   * Uses default system root certificates to protect against Man-in-the-Middle (MITM) attacks.
   * @param sslCtx The Boost SSL context being configured.
   */
  static void VerifyServerCertificate(ssl::context& sslCtx);

  /**
   * @brief Dynamically fetches the active session token and attaches it to the HTTP headers.
   * Mutates the provided request object to include an "Authorization: Bearer <token>" header
   * if a valid identity session exists in the local cache.
   * @param req The outbound Boost HTTP request object.
   */
  void LoadAndAttachSessionToken(http::request<http::string_body>& req) const;

  /**
   * @brief Safely and gracefully shuts down the TLS/SSL stream.
   * @param sslStream The active Boost SSL stream to close.
   */
  static void CloseSSLStream(beast::ssl_stream<beast::tcp_stream>& sslStream);

  /**
   * @brief Evaluates the HTTP response status code and throws domain-specific C++ exceptions.
   * Maps status codes like to their respective Exception classes defined
   * in the networking layer, parsing the error message from the body.
   * @param statusCode The raw integer HTTP status code.
   * @param res The incoming Boost HTTP response.
   */
  static void ThrowOnErrorStatusCode(const unsigned statusCode, http::response<http::string_body>& res);

  /**
   * @brief Extracts the JSON payload from a successful Boost response and maps it.
   * @param statusCode The raw integer HTTP status code.
   * @param res The incoming Boost HTTP response containing the JSON string body.
   * @return A clean, domain-agnostic HttpResponse struct.
   */
  static HttpResponse ParseAndReturnOkResponse(const unsigned statusCode, http::response<http::string_body>& res);

  /**
   * @brief Resolves the hostname to an IP address and establishes the TCP connection.
   * @param sslCtx The configured SSL context.
   * @param resolver The Boost TCP resolver for DNS lookups.
   * @param sslStream The SSL stream wrapper around the TCP socket.
   */
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