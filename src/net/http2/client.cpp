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

static hidi::any_io_context make_client_io_context(starter_t s) {
  return aa::inplaced{[&] { return hidi::asio_io(std::move(s)); }};
}

using span_t = std::span<const std::filesystem::path>;

http2_client::http2_client(http2_client_init init)
    : impl(hidi::endpoint(init.dst, init.dstport),
           hidi::make_ssl_context_for_client(
               init.additional_ssl_cert.empty() ? span_t{} : span_t(&init.additional_ssl_cert, 1)),
           std::move(init.options), make_client_io_context(std::move(init.starter))),
      host(init.host) {
}

}  // namespace tgbm
