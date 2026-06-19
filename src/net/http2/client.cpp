#include "tgbm/net/http2/client.hpp"

#include <http2/asio/factory.hpp>

namespace tgbm {

http2_client::http2_client(std::string_view host, http2_client_options opts, asio::ip::port_type port)
    : http2_client(http2_client_init{
          .host = std::string(host),
          .dst = std::string(host),
          .dstport = port,
          .options = std::move(opts),
      }) {
}

http2_client::http2_client(http2_client_init init)
    : impl(http2::endpoint(init.dst, init.dstport), std::move(init.options),
           [s = std::move(init.starter),
            tcpopts = std::move(init.tcp_options)](asio::io_context& ctx) mutable {
             return http2::any_transport_factory(
                 new http2::asio_tls_factory(ctx, std::move(tcpopts), std::move(s)));
           }),
      host(init.host) {
}

}  // namespace tgbm
