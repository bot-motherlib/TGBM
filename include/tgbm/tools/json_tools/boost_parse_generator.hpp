#pragma once
#include <span>
#include <tgbm/tools/json_tools/generator_parser/basic_parser.hpp>
#include <tgbm/tools/stack_resource.hpp>
#include <boost/json/basic_parser.hpp>

namespace fmt {
template <>
struct formatter<::boost::json::string_view> : formatter<std::string_view> {
  static auto format(::boost::json::string_view sv, auto& ctx) -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}", std::string_view{sv.data(), sv.size()});
  }
};
}  // namespace fmt

namespace tgbm::json::boost {
namespace details {

inline generator_parser::generator generator_starter(generator_parser::generator gen, bool& ended) {
  co_yield {};
  co_yield dd::elements_of(gen);
  ended = true;
  co_yield {};
  TGBM_JSON_PARSE_ERROR;
}

inline generator_parser::generator unite_generate(
    std::vector<char>& buf, bool& part, generator_parser::event_holder& holder,
    generator_parser::generator& field_gen,
    generator_parser::generator old_gen, generator_parser::resource_tag auto r) {
  holder.expect(generator_parser::event_holder::part);
  part = true;
  buf.clear();

  while (holder.got == generator_parser::event_holder::part) {
    buf.insert(buf.end(), holder.str_m.begin(), holder.str_m.end());
    co_yield {};
  }

  buf.insert(buf.end(), holder.str_m.begin(), holder.str_m.end());
  holder.str_m = std::string_view{buf.data(), buf.size()};

  (void)field_gen.release();
  field_gen = std::move(old_gen);

  part = false;

  co_await dd::this_coro::destroy_and_transfer_control_to(field_gen.raw_handle().promise().current_worker);
}

template <typename R>
struct wait_handler {
  using error_code = ::boost::json::error_code;
  using string_view = ::boost::json::string_view;

  static constexpr std::size_t max_array_size = -1;
  static constexpr std::size_t max_object_size = -1;
  static constexpr std::size_t max_string_size = -1;
  static constexpr std::size_t max_key_size = -1;

  using wait_e = generator_parser::event_holder::wait_e;
  using gen_t = generator_parser::generator;

  generator_parser::event_holder event;
  std::vector<char> buf;
  gen_t gen;
  R* memory_resource;
  bool part = false;
  bool ended = false;

  auto resource() noexcept{
    return dd::with_resource<R>(*memory_resource);
  }

  template <typename T>
  static gen_t simple_generator(T& t_, generator_parser::event_holder& event){
    generator_parser::boost_domless_parser<T>::simple_parse(t_, event);
    co_return;
  }

  template <typename T>
  wait_handler(T& t_, R& res)
      :  memory_resource(&res) {
    
    gen_t parse_gen;
    if constexpr (!generator_parser::boost_domless_parser<T>::simple){
      parse_gen =  generator_parser::boost_domless_parser<T>::parse(t_, event, resource());
    }
    else {  
      parse_gen = simple_generator(t_, event);
    }

    gen = generator_starter(std::move(parse_gen), ended);
    gen.begin();
  }

  wait_handler(wait_handler&&) = delete;

  bool on_document_begin(error_code& ec) {
    return true;
  }

  bool on_document_end(error_code& ec) {
    return true;
  }

  void resume_generator() {
    auto it = dd::generator_iterator<generator_parser::nothing_t>(gen);
    ++it;
  }

  bool on_array_begin(error_code& ec) {
    event.got = wait_e::array_begin;
    resume_generator();
    return true;
  }

  bool on_array_end(std::size_t n, error_code& ec) {
    event.got = wait_e::array_end;
    resume_generator();
    return true;
  }

  bool on_object_begin(error_code& ec) {
    event.got = wait_e::object_begin;
    resume_generator();
    return true;
  }

  bool on_object_end(std::size_t n, error_code& ec) {
    event.got = wait_e::object_end;
    resume_generator();
    return true;
  }

  bool on_string(string_view s, std::size_t n, error_code& ec) {
    event.got = wait_e::string;
    event.str_m = s;
    resume_generator();
    return true;
  }

  bool on_key(string_view s, std::size_t n, error_code& ec) {
    event.got = wait_e::key;
    event.str_m = s;
    resume_generator();
    return true;
  }

  bool on_int64(int64_t i, string_view s, error_code& ec) {
    event.got = wait_e::int64;
    event.int_m = i;
    resume_generator();
    return true;
  }

  bool on_uint64(uint64_t u, string_view s, error_code& ec) {
    event.got = wait_e::uint64;
    event.uint_m = u;
    resume_generator();
    return true;
  }

  bool on_double(double d, string_view s, error_code& ec) {
    event.got = wait_e::double_;
    event.double_m = d;
    resume_generator();
    return true;
  }

  bool on_bool(bool b, error_code& ec) {
    event.got = wait_e::bool_;
    event.bool_m = b;
    resume_generator();
    return true;
  }

  bool on_null(error_code& ec) {
    event.got = wait_e::null;
    resume_generator();
    return true;
  }

  void on_part(string_view s) {
    event.got = generator_parser::event_holder::part;
    event.str_m = s;
    if (!part) {
      gen = unite_generate(buf, part, event, gen, std::move(gen), resource());
      gen.prepare_to_start();
    }
    resume_generator();
  }

  bool on_string_part(string_view s, std::size_t n, error_code& ec) {
    on_part(s);
    return true;
  }

  bool on_comment_part(string_view s, error_code& ec) {
    return true;
  }

  bool on_key_part(string_view s, std::size_t n, error_code& ec) {
    on_part(s);
    return true;
  }

  bool on_number_part(string_view s, error_code& ec) {
    return true;
  }

  bool on_comment(string_view s, error_code& ec) {
    return true;
  }
};

}  // namespace details 

namespace unsafe{
  inline std::vector<unsigned char> s(1024 * 1024 * 8, 0);

  inline std::span<unsigned char> arena_for_generators(){
    return {s.data(), s.size()};
  }

  inline stack_resource resource{arena_for_generators()};

  inline stack_resource* arena_resource(){
    
    return &resource;
  }

  struct global_arena_resource{
    static void* allocate(std::size_t bytes, std::size_t align){
      return arena_resource()->allocate(bytes, align);
    }
    static void deallocate(void* ptr, std::size_t bytes, std::size_t align) noexcept{
      return arena_resource()->deallocate(ptr, bytes, align);
    }
  };
  static_assert(dd::memory_resource<global_arena_resource>);
  static_assert(dd::last_is_memory_resource_tag<dd::with_resource<global_arena_resource>>);
  static_assert(std::is_empty_v<global_arena_resource>);
}

template <typename T, typename R = unsafe::global_arena_resource>
T parse_generator(std::string_view data, R r = {}) {
  T t;
  ::boost::json::basic_parser<details::wait_handler<R>> p{::boost::json::parse_options{}, t, r};
  ::boost::json::error_code ec;
  p.write_some(false, data.data(), data.size(), ec);
  if (ec || !p.handler().ended) {
    TGBM_JSON_PARSE_ERROR;
  }
  return t;
}

template <typename T, typename R = unsafe::global_arena_resource>
struct stream_parser {
  TGBM_PIN;

  ::boost::json::basic_parser<details::wait_handler<R>> p;
  ::boost::json::error_code ec;  

  explicit stream_parser(T& t, R r = {}) : p(::boost::json::parse_options{}, t, r) {
  }

  void parse(std::string_view data, bool end) {
    p.write_some(!end, data.data(), data.size(), ec);
    if (ec || (end && !p.handler().ended)) {
      LOG_ERR("ec: {}, ended: {}", ec.message(), p.handler().ended);
      TGBM_JSON_PARSE_ERROR;
    }
  }
};

}  // namespace tgbm::json::boost
