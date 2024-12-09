#pragma once

#include <span>

#include <kelcoro/task.hpp>

#include <anyany/anyany.hpp>

#include <tgbm/net/errors.hpp>
#include <tgbm/utils/memory.hpp>

namespace tgbm {

struct async_read_m {
  static dd::task<void> do_invoke(auto& self, std::span<byte_t> buf, io_error_code& ec) {
    return self.read(buf, ec);
  }

  template <typename CRTP>
  struct plugin {
    // TODO нужно изменить интерфейс на read_some ? Или... Нет?
    dd::task<void> read(std::span<byte_t> buf, io_error_code& ec) {
      return aa::invoke<async_read_m>(static_cast<CRTP&>(*this), buf, ec);
    }
  };
};

struct async_write_m {
  static dd::task<size_t> do_invoke(auto& self, std::span<const byte_t> buf, io_error_code& ec) {
    return self.write(buf, ec);
  }

  template <typename CRTP>
  struct plugin {
    dd::task<size_t> write(std::span<const byte_t> buf, io_error_code& ec) {
      return aa::invoke<async_write_m>(static_cast<CRTP&>(*this), buf, ec);
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
    aa::basic_any_with<aa::default_allocator, 0, async_read_m, async_write_m, shutdown_m, yield_m>;

}  // namespace tgbm
