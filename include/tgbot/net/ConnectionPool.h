#pragma once

#include <cassert>
#include <mutex>
#include <coroutine>
#include <anyany/anyany.hpp>

#include <kelcoro/task.hpp>

#include <cassert>

// TODO change namespac TgBot -> tgbm
namespace TgBot {

template <typename Node>
struct intrusive_stack {
  Node *first = nullptr;

  [[nodiscard]] bool empty() const noexcept {
    return !first;
  }

  void push(Node *n) noexcept {
    assert(n);
    if (!first) {
      first = n;
      return;
    }
    n->next = std::exchange(first, n);
  }

  [[nodiscard]] Node *pop() noexcept {
    assert(first);
    return std::exchange(first, first->next);
  }
};

template <typename T>
struct pool_t {
 private:
  using data_type = T;
  using factory_t = aa::any_with<aa::call<dd::task<data_type>()>>;
  struct node_t {
    node_t *next = nullptr;
    data_type data;
  };
  struct waiter_node : dd::task_node {
    node_t *result = nullptr;
  };

  intrusive_stack<node_t> free;
  size_t max;
  size_t free_count;
  size_t borrowed_count;
  intrusive_stack<dd::task_node> waiters;
  factory_t factory;
  dd::any_executor_ref exe;  // where waiters will be resumed
  mutable std::mutex mtx;

  struct free_data_awaiter {
    pool_t &pool;
    waiter_node node;

    static bool await_ready() noexcept {
      return false;
    }
    bool await_suspend(std::coroutine_handle<> handle) noexcept {
      node.task = handle;
      std::lock_guard l(pool.mtx);
      if (!pool.free.empty()) {
        node.result = pool.free.pop();
        return false;
      }
      pool.waiters.push(&node);
      return true;
    }
    [[nodiscard]] node_t *await_resume() const noexcept {
      return node.result;
    }
  };
  static std::pmr::memory_resource *resource() noexcept {
    return std::pmr::new_delete_resource();
  }

 public:
  // attach operation on 'e' should be thread safe, 'e' used to resume waiters on borrow
  explicit pool_t(size_t max_count, auto &&fac, dd::any_executor_ref e)
      : max(max_count), free_count(0), borrowed_count(0), factory(static_cast<decltype(fac)>(fac)), exe(e) {
    assert(factory);
  }

  ~pool_t() {
    assert(borrowed_count == 0);
    assert(free_count <= max);
    assert(waiters.empty());
    while (!free.empty()) {
      node_t *c = free.pop();
      resource()->deallocate(c, sizeof(node_t), alignof(node_t));
    }
  }

  struct handle_t {
   private:
    friend struct pool_t;

    node_t *node = nullptr;
    pool_t *p = nullptr;

    explicit handle_t(node_t *n, pool_t &pool) noexcept : node(n), p(&pool) {
    }

   public:
    handle_t(handle_t &&other) noexcept : node(std::exchange(other.node, nullptr)), p(other.p) {
    }
    handle_t &operator=(handle_t &&other) noexcept {
      std::swap(p, other.p);
      std::swap(node, other.node);
      return *this;
    }
    ~handle_t() {
      if (node)
        p->payoff(node);
    }
    data_type *get() noexcept {
      return node ? std::addressof(node->data) : nullptr;
    }
    const data_type *get() const noexcept {
      return node ? std::addressof(node->data) : nullptr;
    }
  };
  dd::task<handle_t> borrow() {
    std::unique_lock<std::mutex> l(mtx);
    if (!free.empty()) {
      ++borrowed_count;
      --free_count;
      co_return handle_t(free.pop(), *this);
    }
    if (borrowed_count == max) {
      l.unlock();
      co_return handle_t(co_await free_data_awaiter(*this), *this);
    }
    ++borrowed_count;
    l.unlock();
    // new_delete resource do not require synchronization
    void *mem = resource()->allocate(sizeof(node_t), alignof(node_t));
    node_t *node = new (mem) node_t(nullptr, co_await factory());
    co_return handle_t(node, *this);
  }

 private:
  void payoff(node_t *node) noexcept {
    assert(node);
    std::unique_lock l(mtx);
    if (!waiters.empty()) {
      waiter_node *waiter = static_cast<waiter_node *>(waiters.pop());
      l.unlock();
      waiter->result = node;
      exe.attach(waiter);
    } else {
      --borrowed_count;
      ++free_count;
      assert(free_count <= max && "not borrowed node returned");
      free.push(node);
    }
  }
};

}  // namespace TgBot
