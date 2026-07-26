#include "tgbm/net/http2/client.hpp"

#include <hidi/asio/io_impl.hpp>

namespace tgbm {

http2_client::http2_client(std::string_view host, http2_client_options opts, asio::ip::port_type port)
    : http2_client(http2_client_init{
          .host = std::string(host),
          .dst = std::string(host),
          .dstport = port,
          .options = std::move(opts),
      }) {
}

static hidi::any_io_context make_client_ssl_context(hidi::tcp_connection_options tcp_options, starter_t s) {
  return aa::inplaced{
      [&] { return hidi::asio_tls_io(hidi::make_ssl_context_for_client({}), tcp_options, std::move(s)); }};
}

http2_client::http2_client(http2_client_init init)
    : impl(hidi::endpoint(init.dst, init.dstport), std::move(init.options),
           make_client_ssl_context(init.tcp_options, std::move(init.starter))),
      host(init.host) {
}

}  // namespace tgbm
