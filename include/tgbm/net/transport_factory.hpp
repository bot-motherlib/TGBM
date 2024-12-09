#pragma once

#include <kelcoro/task.hpp>

#include <tgbm/utils/memory.hpp>
#include <tgbm/net/errors.hpp>
#include <tgbm/utils/deadline.hpp>

#include <tgbm/net/any_connection.hpp>

namespace tgbm {

struct create_connection_m {
  static dd::task<any_connection> do_invoke(auto& self, std::string_view host) {
    return self.create_connection(host);
  }

  template <typename CRTP>
  struct plugin {
    dd::task<any_connection> create_connection(std::string_view host) {
      return aa::invoke<create_connection_m>(static_cast<CRTP&>(*this), host);
    }
  };
};

struct run_m {
  static void do_invoke(auto& self) {
    return self.run();
  }

  template <typename CRTP>
  struct plugin {
    void run() {
      return aa::invoke<run_m>(static_cast<CRTP&>(*this));
    }
  };
};

struct stop_m {
  static void do_invoke(auto& self) {
    return self.stop();
  }

  template <typename CRTP>
  struct plugin {
    void stop() {
      return aa::invoke<stop_m>(static_cast<CRTP&>(*this));
    }
  };
};

struct run_one_m {
  static bool do_invoke(auto& self, duration_t timeout) {
    return self.run_one(timeout);
  }

  template <typename CRTP>
  struct plugin {
    bool run_one(duration_t timeout) {
      return aa::invoke<run_one_m>(static_cast<CRTP&>(*this), timeout);
    }
  };
};

struct sleep_m {
  static dd::task<void> do_invoke(auto& self, duration_t d, io_error_code& ec) {
    return self.sleep(d, ec);
  }

  template <typename CRTP>
  struct plugin {
    dd::task<void> sleep(duration_t d, io_error_code& ec) {
      return aa::invoke<sleep_m>(static_cast<CRTP&>(*this), d, ec);
    }
  };
};

/*
required intereface:

  dd::task<any_connection> create_connection(std::string_view host);
  void run();
  void stop();
  bool run_one(duration_t timeout);
  dd::task<void> sleep(duration_t d, io_error_code& ec);
*/
using any_transport_factory =
    aa::basic_any_with<aa::default_allocator, 0, create_connection_m, run_m, stop_m, run_one_m, sleep_m>;

[[nodiscard]] any_transport_factory default_transport_factory();

}  // namespace tgbm
