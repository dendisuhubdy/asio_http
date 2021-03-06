/**
    asio_http: http client library for boost asio
    Copyright (c) 2017 Julio Becerra Gomez
    See COPYING for license information.
*/

#ifndef ASIO_HTTP_HTTP_CLIENT_H
#define ASIO_HTTP_HTTP_CLIENT_H

#include "asio_http/http_client_settings.h"
#include "asio_http/internal/request_data.h"
#include "asio_http/internal/request_manager.h"
#include <asio_http/http_request.h>

#include <boost/asio.hpp>
#include <memory>
#include <thread>

namespace asio_http
{
class http_client
{
public:
  explicit http_client(const http_client_settings& settings);

  http_client(const http_client_settings& settings, boost::asio::io_context& io_context);

  virtual ~http_client();

  template<typename CompletionToken>
  auto execute_request(CompletionToken&&                             completion_token,
                       std::shared_ptr<const http_request_interface> request,
                       const std::string&                            cancellation_token)
  {
    boost::asio::async_completion<CompletionToken, void(http_request_result)> init{ completion_token };

    internal::request_data new_request(
      std::move(request),
      init.completion_handler,
      boost::asio::get_associated_executor(init.completion_handler, m_request_manager->get_strand()),
      cancellation_token);

    m_request_manager->get_strand().post(
      [ptr = m_request_manager, request = std::move(new_request)]() { ptr->execute_request(request); });

    return init.result.get();
  }

  template<typename CompletionToken>
  auto get(CompletionToken&& completion_token, const std::string& url_string, const std::string& cancellation_token)
  {
    const auto request = std::make_shared<http_request>(http_request_interface::http_method::GET,
                                                        url(url_string),
                                                        std::string(),
                                                        http_request::DEFAULT_TIMEOUT_MSEC,
                                                        ssl_settings(),
                                                        std::vector<std::string>(),
                                                        std::vector<std::uint8_t>(),
                                                        http_request_interface::compression_policy::never);

    return execute_request(std::forward<CompletionToken>(completion_token), request, cancellation_token);
  }

  template<typename CompletionToken>
  auto get(CompletionToken&& completion_token, const std::string& url_string)
  {
    return get(std::forward<CompletionToken>(completion_token), url_string, "");
  }

  template<typename CompletionToken>
  auto post(CompletionToken&&         completion_token,
            const std::string&        url_string,
            std::vector<std::uint8_t> data,
            const std::string&        content_type,
            const std::string&        cancellation_token)
  {
    const auto request = std::make_shared<http_request>(http_request_interface::http_method::POST,
                                                        url(url_string),
                                                        std::string(),
                                                        http_request::DEFAULT_TIMEOUT_MSEC,
                                                        ssl_settings(),
                                                        std::vector<std::string>{ "Content-Type: " + content_type },
                                                        std::move(data),
                                                        http_request_interface::compression_policy::never);

    return execute_request(std::forward<CompletionToken>(completion_token), request, cancellation_token);
  }

  template<typename CompletionToken>
  auto post(CompletionToken&&         completion_token,
            const std::string&        url_string,
            std::vector<std::uint8_t> data,
            const std::string&        content_type)
  {
    return post(std::forward<CompletionToken>(completion_token), url_string, data, content_type, "");
  }

  void cancel_requests(const std::string& cancellation_token);

private:
  void shut_down_io_context();
  void io_context_thread();

  // Declaration (initialization) order is relevant for the members below
  boost::asio::io_context                    m_internal_io_context;
  boost::asio::io_context&                   m_io_context;
  std::thread                                m_io_thread;
  std::shared_ptr<internal::request_manager> m_request_manager;
};
}  // namespace asio_http

#endif
