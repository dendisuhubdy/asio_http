/**
    asio_http: http client library for boost asio
    Copyright (c) 2017 Julio Becerra Gomez
    See COPYING for license information.
*/

#ifndef ASIO_HTTP_HTTP_REQUEST_MANAGER_SETTINGS_H
#define ASIO_HTTP_HTTP_REQUEST_MANAGER_SETTINGS_H

#include <cinttypes>

namespace asio_http
{
struct http_client_settings
{
  http_client_settings()
      : max_parallel_requests(25)
  {
  }
  http_client_settings(std::uint32_t max_parallel_requests_)
      : max_parallel_requests(max_parallel_requests_)
  {
  }
  const std::uint32_t max_parallel_requests;
};
}  // namespace asio_http
#endif
