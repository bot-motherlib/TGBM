#include <tgbm/net/transport_factory.hpp>

#include <tgbm/net/asio_tls_transport.hpp>

namespace tgbm {

any_transport_factory default_transport_factory() {
  return $inplace(asio_tls_transport());
}

}  // namespace tgbm
