#include <tgbm/net/transport_factory.hpp>

#include <tgbm/net/asio_transport_factory.hpp>

namespace tgbm {

any_transport_factory default_transport_factory() {
  return asio_transport_factory{};
}

}  // namespace tgbm
