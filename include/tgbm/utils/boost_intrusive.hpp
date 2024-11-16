#pragma once

#include <boost/intrusive/link_mode.hpp>
#include <boost/intrusive/options.hpp>
#include <boost/intrusive/trivial_value_traits.hpp>

#include <kelcoro/executor_interface.hpp>

namespace tgbm {

namespace bi = boost::intrusive;

template <typename IntrusiveContainer>
static void erase_byref(IntrusiveContainer& c, typename IntrusiveContainer::value_type& node) noexcept {
  if constexpr (requires { IntrusiveContainer::s_iterator_to(node); }) {
    c.erase(IntrusiveContainer::s_iterator_to(node));
  } else {
    c.erase(c.iterator_to(node));
  }
}
// i need to know is node linked or not, so always use safe_link (container resets node on erasing)
constexpr inline bi::link_mode_type default_link_mode = bi::safe_link;

using link_option = bi::link_mode<default_link_mode>;

// traits for dd::task_node to use it as hook for slist containers
struct task_node_traits {
  using node = dd::task_node;
  using node_ptr = node*;
  using const_node_ptr = const node*;

  static node* get_next(const node* n) {
    return n->next;
  }
  static void set_next(node* n, node* next) {
    n->next = next;
  }
};
using task_node_value_traits = bi::trivial_value_traits<task_node_traits, default_link_mode>;

using task_node_traits_option = bi::value_traits<task_node_value_traits>;

}  // namespace tgbm
