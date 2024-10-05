#pragma once
#include <cassert>
#include <coroutine>
#include <anyany/anyany.hpp>
#include <kelcoro/task.hpp>
#include <cassert>
#include <mutex>

#include "tgbm/tools/scope_exit.h"

#include <boost/intrusive/list_hook.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/slist.hpp>
#include <boost/intrusive/slist_hook.hpp>

#include "tgbm/tools/boost_intrusive.hpp"

#include "tgbm/logger.h"

#include "tgbm/tools/macro.hpp"

namespace tgbm {

template <typename EventHandler, typename T>
concept pool_events_handler_for = requires(EventHandler& e, T& t) {
  EventHandler::dropped(t);
  EventHandler::reused(t);
  EventHandler::created(t);
  EventHandler::deleted(t);
};

template <typename T>
struct noop_events_handler_t {
  static void dropped(const T&) {
  }
  static void reused(const T&) {
  }
  static void created(const T&) {
  }
  static void deleted(const T&) {
  }
};

// TODO into another file or smth like, utils
struct null_mutex {
  static constexpr void lock() noexcept {
  }
  static constexpr void unlock() noexcept {
  }
  static constexpr bool try_lock() noexcept {
    return true;
  }
};

// TODO coro allocator multithread (interface)
template <typename T, pool_events_handler_for<T> H = noop_events_handler_t<T>>
struct pool_t {
 private:
  using data_type = T;
  using factory_t = aa::any_with<aa::call<dd::task<data_type>()>>;

  struct node_t {
    data_type data;
    bi::slist_member_hook<link_option> free_connections_hook = {};
    TGBM_PIN;
  };
  struct waiter_node : bi::list_base_hook<link_option> {
    pool_t* pool = nullptr;
    node_t* result = nullptr;
    std::coroutine_handle<> task;
    TGBM_PIN;

    explicit waiter_node(pool_t& p) noexcept : pool(&p) {
    }

    static bool await_ready() noexcept {
      return false;
    }
    void await_suspend(std::coroutine_handle<> handle) noexcept {
      assert(pool->free.empty());
      pool->waiters.push_back(*this);
      task = handle;
    }
    [[nodiscard]] node_t& await_resume() const {
      if (!result)
        throw std::invalid_argument{""};  // TODO better (or rm?)
      H::reused(result->data);
      return *result;
    }
  };

  using free_connections_hook_option =
      bi::member_hook<node_t, bi::slist_member_hook<link_option>, &node_t::free_connections_hook>;
  using waiters_t = bi::list<waiter_node, bi::cache_begin<true>, bi::cache_last<true>>;

  bi::slist<node_t, free_connections_hook_option, bi::constant_time_size<true>> free;
  size_t max = 0;
  size_t borrowed_count = 0;
  waiters_t waiters;
  factory_t factory;

  [[nodiscard]] waiter_node wait_free() noexcept {
    return waiter_node(*this);
  }
  static void destroy_node(node_t* n) noexcept {
    assert(n);
    H::deleted(n->data);
    std::destroy_at(n);
    resource()->deallocate(n, sizeof(node_t), alignof(node_t));
  }
  static std::pmr::memory_resource* resource() noexcept {
    return std::pmr::new_delete_resource();
  }

 public:
  // attach operation on 'e' should be thread safe, 'e' used to resume waiters on borrow
  explicit pool_t(size_t max_count, auto&& fac)
      : max(std::max<size_t>(1, max_count)), borrowed_count(0), factory(static_cast<decltype(fac)>(fac)) {
    assert(factory);
  }

  ~pool_t() {
    assert(empty());
  }

  struct handle_t {
   private:
    friend struct pool_t;

    node_t* node = nullptr;
    pool_t* p = nullptr;

    explicit handle_t(node_t& n, pool_t& pool) noexcept : node(&n), p(&pool) {
    }

   public:
    handle_t() = default;
    handle_t(handle_t&& other) noexcept : node(std::exchange(other.node, nullptr)), p(other.p) {
    }
    handle_t& operator=(handle_t&& other) noexcept {
      std::swap(p, other.p);
      std::swap(node, other.node);
      return *this;
    }
    ~handle_t() {
      if (node)
        p ? p->payoff(node) : destroy_node(node);
    }
    data_type* get() noexcept {
      return node ? std::addressof(node->data) : nullptr;
    }
    const data_type* get() const noexcept {
      return node ? std::addressof(node->data) : nullptr;
    }
    // pool forgets about borrowed data forever
    void drop() noexcept {
      if (!node)
        return;
      H::dropped(node->data);
      if (!p)
        return;
      --p->borrowed_count;
      p = nullptr;
    }
    [[nodiscard]] bool dropped() const noexcept {
      return !p && node;
    }
    [[nodiscard]] bool empty() const noexcept {
      return !node;
    }
  };

  [[nodiscard]] size_t max_count() const noexcept {
    return max;
  }

  // returns empty handle if pool shutted down
  dd::task<handle_t> borrow() {
    if (!free.empty()) {
      assert(borrowed_count < max);
      ++borrowed_count;
      node_t& n = free.front();
      free.pop_front();
      H::reused(n.data);
      co_return handle_t(n, *this);
    }
    if (borrowed_count == max) {
      node_t& n = co_await wait_free();
      co_return handle_t(n, *this);
    }
    void* mem = resource()->allocate(sizeof(node_t), alignof(node_t));
    on_scope_failure(free_mem) {
      LOG_DEBUG("{} CALLED!!!", mem);
      --borrowed_count;
      resource()->deallocate(mem, sizeof(node_t), alignof(node_t));
    };
    ++borrowed_count;
    auto res = co_await factory();  // not inplace because GCC, internal compiler error
    node_t* node = new (mem) node_t(std::move(res));
    H::created(node->data);
    free_mem.no_longer_needed();
    co_return handle_t(*node, *this);
  }
  // stops all connections
  void shutdown() {
    auto f = std::move(free);
    f.clear_and_dispose([](node_t* n) { destroy_node(n); });
    auto w = std::move(waiters);
    w.clear_and_dispose([&](waiter_node* n) { n->task.resume(); });
    assert(free.empty());
    assert(waiters.empty());
  }
  [[nodiscard]] bool empty() const noexcept {
    return borrowed_count == 0 && free.size() == 0;
  }

 private:
  void payoff(node_t* node) noexcept {
    assert(node);
    assert(borrowed_count != 0);
    if (!waiters.empty()) {
      waiter_node& waiter = waiters.front();
      waiters.pop_front();
      waiter.result = node;
      waiter.task.resume();
    } else {
      --borrowed_count;
      free.push_front(*node);
    }
  }
};
}  // namespace tgbm
