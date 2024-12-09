#pragma once

#include <span>

#include <kelcoro/task.hpp>

#include <anyany/anyany.hpp>

#include <tgbm/net/errors.hpp>
#include <tgbm/utils/memory.hpp>

namespace tgbm {

struct start_read_m {
  static void do_invoke(auto& self, std::coroutine_handle<> callback, std::span<byte_t> buf,
                        io_error_code& ec) {
    return self.start_read(callback, buf, ec);
  }

  template <typename CRTP>
  struct plugin {
    void start_read(std::coroutine_handle<> callback, std::span<byte_t> buf, io_error_code& ec) {
      return aa::invoke<start_read_m>(static_cast<CRTP&>(*this), callback, buf, ec);
    }
  };
};

// must send 'buf', set 'ec' if needed, set 'written' to count of bytes written
struct start_write_m {
  static void do_invoke(auto& self, std::coroutine_handle<> callback, std::span<const byte_t> buf,
                        io_error_code& ec, size_t& written) {
    return self.start_write(callback, buf, ec, written);
  }

  template <typename CRTP>
  struct plugin {
    void start_write(std::coroutine_handle<> callback, std::span<const byte_t> buf, io_error_code& ec,
                     size_t& written) {
      return aa::invoke<start_write_m>(static_cast<CRTP&>(*this), callback, buf, ec, written);
    }
  };
};

struct shutdown_m {
  static void do_invoke(auto& self) {
    return self.shutdown();
  }

  template <typename CRTP>
  struct plugin {
    void shutdown() noexcept {
      return aa::invoke<shutdown_m>(static_cast<CRTP&>(*this));
    }
  };
};

struct yield_m {
  static dd::task<void> do_invoke(auto& self) {
    return self.yield();
  }

  template <typename CRTP>
  struct plugin {
    dd::task<void> yield() {
      return aa::invoke<yield_m>(static_cast<CRTP&>(*this));
    }
  };
};

/*
  required interface:

    dd::task<void> read(std::span<byte_t> buf, io_error_code& ec);
    dd::task<size_t> write(std::span<const byte_t> buf, io_error_code& ec);
    void shutdown() noexcept;
    dd::task<void> yield();

*/
using any_connection =
    aa::basic_any_with<aa::default_allocator, 0, start_read_m, start_write_m, shutdown_m, yield_m>;

}  // namespace tgbm
