#pragma once

#include <span>

#include <kelcoro/task.hpp>

#include <anyany/anyany.hpp>

#include <tgbm/net/errors.hpp>
#include <tgbm/utils/memory.hpp>

#include <tgbm/utils/anyany_utils.hpp>

namespace tgbm {

ANYANY_METHOD(start_read, void(std::coroutine_handle<> callback, std::span<byte_t> buf, io_error_code& ec));

// must send 'buf', set 'ec' if needed, set 'written' to count of bytes written (before callback invoked)
ANYANY_METHOD(start_write, void(std::coroutine_handle<> callback, std::span<const byte_t> buf,
                                io_error_code& ec, size_t& written));

ANYANY_METHOD(shutdown, void());

struct is_https_m {
  using value_type = bool;

  template <typename T>
  static consteval bool do_value() {
    return T::is_https;
  }

  template <typename CRTP>
  struct plugin {
    bool is_https() const noexcept {
      return aa::invoke<is_https_m>(static_cast<const CRTP&>(*this));
    }
  };
};

/*
  required interface:

    void start_read(std::coroutine_handle<> callback, std::span<byte_t> buf, io_error_code& ec);
    void start_write(std::coroutine_handle<> callback, std::span<const byte_t> buf,
                                io_error_code& ec, size_t& written);
    void shutdown() noexcept;
    static constexpr bool is_https = ...;
*/
using any_connection =
    aa::basic_any_with<aa::default_allocator, 0, start_read_m, start_write_m, shutdown_m, is_https_m>;

// awaiters for using with .start_write / .start_read

struct read_awaiter {
  any_connection& con;
  io_error_code& ec;
  std::span<byte_t> buf;

  static bool await_ready() noexcept {
    return false;
  }

  void await_suspend(std::coroutine_handle<> h) const {
    con.start_read(h, buf, ec);
  }
  static void await_resume() noexcept {
  }
};

struct write_awaiter {
  any_connection& con;
  io_error_code& ec;
  std::span<const byte_t> buf;
  size_t written;

  static bool await_ready() noexcept {
    return false;
  }

  void await_suspend(std::coroutine_handle<> h) {
    con.start_write(h, buf, ec, written);
  }
  size_t await_resume() noexcept {
    return written;
  }
};

}  // namespace tgbm
