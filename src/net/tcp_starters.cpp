#include "tgbm/net/tcp_starters.hpp"

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include <http2/asio/awaiters.hpp>

namespace ip = boost::asio::ip;
using http2::byte_t;
using http2::net;

namespace tgbm {

template <typename... Args>
[[noreturn]] static void fail(std::format_string<Args...> fmt, Args&&... args) {
  throw std::runtime_error(std::format(fmt, std::forward<Args>(args)...));
}

constexpr byte_t VER = 0x05;
constexpr byte_t CMD_CONNECT = 0x01;
constexpr byte_t RSV = 0x00;  // reserved

enum auth_methods_e : byte_t {
  NO_AUTH = 0,
};
enum dst_type_e : byte_t {
  IPV4 = 0x01,
  IPV6 = 0x04,
  FQDN = 0x03,
};

enum reply_statue_e : byte_t {
  request_granted = 0x00,
  general_failure = 0x01,
  connection_not_allowed_by_ruleset = 0x02,
  network_unreachable = 0x03,
  host_unreachable = 0x04,
  connection_refused_by_destination_host = 0x05,
  ttl_expired = 0x06,
  command_not_supported_or_protocol_error = 0x07,
  address_type_not_supported = 0x08,
};

static std::vector<byte_t> make_connection_request(const std::string& target_host, uint16_t target_port) {
  std::vector<byte_t> conreq;
  conreq.reserve(300);

  conreq.push_back(VER);
  conreq.push_back(CMD_CONNECT);
  conreq.push_back(RSV);
  boost::system::error_code ec;
  if (auto addr = ip::make_address_v4(target_host, ec); !ec) {
    conreq.push_back(IPV4);

    auto bytes = addr.to_bytes();
    conreq.insert(conreq.end(), bytes.begin(), bytes.end());
  } else if (auto addr6 = ip::make_address_v6(target_host, ec); !ec) {
    conreq.push_back(IPV6);

    auto bytes = addr6.to_bytes();
    conreq.insert(conreq.end(), bytes.begin(), bytes.end());
  } else {
    if (target_host.size() > 255)
      fail("domain name too long");
    conreq.push_back(FQDN);
    conreq.push_back(static_cast<byte_t>(target_host.size()));

    conreq.insert(conreq.end(), target_host.begin(), target_host.end());
  }

  conreq.push_back(static_cast<byte_t>(target_port >> 8));
  conreq.push_back(static_cast<byte_t>(target_port & 0xff));

  return conreq;
}

dd::task<void> start_socks5(boost::asio::ip::tcp::socket& socket, std::string target_host,
                            uint16_t target_port, http2::deadline_t deadline) {
  http2::io_error_code ec;
  bool timed_out = false;
  boost::asio::steady_timer timer(socket.get_executor());
  timer.expires_at(deadline.tp);
  timer.async_wait([&](const boost::system::error_code& timerec) {
    if (timerec != boost::asio::error::operation_aborted) {
      timed_out = true;
      socket.cancel();
    }
  });
  auto fail_if_ec = [&] {
    if (ec)
      fail("network err: `{}`", ec.message());
    if (timed_out)
      fail("socks5: timeout");
  };

  const byte_t greeting[] = {VER, /*count supported auth methods*/ 1, NO_AUTH};

  co_await net.write(socket, std::span(greeting), ec);
  fail_if_ec();
  std::array<byte_t, 2> resp;
  co_await net.read(socket, std::span(resp), ec);
  fail_if_ec();
  if (resp[0] != VER || resp[1] != NO_AUTH)
    fail("unexpected greeting response = {:X}", std::bit_cast<uint16_t>(resp));

  std::vector<byte_t> conreq = make_connection_request(target_host, target_port);

  co_await net.write(socket, conreq, ec);
  fail_if_ec();
  // CONNECT reply header
  std::array<byte_t, 4> hdr;
  co_await net.read(socket, hdr, ec);
  fail_if_ec();
  if (hdr[0] != VER)
    fail("invalid connect reply header = {:X}", std::bit_cast<uint32_t>(hdr));
  if (hdr[1] != request_granted)
    fail("connect reply status not ok, status = {:X}", hdr[1]);

  const auto atyp = hdr[3];

  // Consume BND.ADDR + BND.PORT
  switch (atyp) {
    case IPV4: {
      std::array<byte_t, 4 + 2> buf;
      co_await net.read(socket, buf, ec);
      fail_if_ec();
      break;
    }
    case IPV6: {
      std::array<byte_t, 16 + 2> buf;
      co_await net.read(socket, buf, ec);
      fail_if_ec();
      break;
    }
    case FQDN: {
      byte_t len;
      co_await net.read(socket, std::span(&len, 1), ec);
      fail_if_ec();
      std::vector<byte_t> buf(len + 2);
      co_await net.read(socket, buf, ec);
      fail_if_ec();
      break;
    }
    default:
      fail("invalid ATYP in CONNECT response, ATYP = {:X}", atyp);
  }
}

[[nodiscard]] starter_t socks5_starter(std::string target_host, uint16_t target_port) {
  return [=](boost::asio::ip::tcp::socket& socket, http2::deadline_t deadline) {
    return start_socks5(socket, target_host, target_port, deadline);
  };
}

}  // namespace tgbm
