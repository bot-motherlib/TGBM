#pragma once

#include <boost/json.hpp>

#include "boost/json/serialize_options.hpp"
#include "tgbm/tools/box.hpp"
#include "tgbm/tools/pfr_extension.hpp"
#include "tgbm/tools/traits.hpp"
#include "tgbm/api/common.hpp"
#include "tgbm/tools/json_tools/exceptions.hpp"
#include "tgbm/tools/api_utils.hpp"

namespace boost::json {

template <typename T>
void tag_invoke(value_from_tag, value& jv, const std::optional<T>& val) {
  if (val) {
    jv = boost::json::value_from(*val);
  }
}

template <typename T>
void tag_invoke(value_from_tag, value& jv, const tgbm::api::optional<T>& val) {
  if (val) {
    jv = boost::json::value_from(*val);
  }
}

template <typename T>
void tag_invoke(value_from_tag, value& jv, const tgbm::box<T>& val) {
  if (val) {
    jv = boost::json::value_from(*val);
  }
}

inline void tag_invoke(value_from_tag, value& jv, tgbm::api::True val) {
  jv = boost::json::value_from(true);
}

inline void tag_invoke(value_from_tag, value& jv, tgbm::api::Double val) {
  jv = boost::json::value_from(val.value);
}

inline void tag_invoke(value_from_tag, value& jv, tgbm::api::Integer val) {
  jv = boost::json::value_from(val.value);
}

inline void tag_invoke(value_from_tag, value& jv, tgbm::nothing_t) {
  jv = boost::json::object{};
}

inline void tag_invoke(value_from_tag, value& jv, const tgbm::const_string& str) {
  jv = str.str();
}

template <tgbm::common_api_type T>
inline void tag_invoke(value_from_tag, value& jv, const T& t) {
  if (!jv.is_object()) {
    jv = boost::json::object{};
  }
  auto& obj = jv.as_object();
  pfr_extension::visit_object(t, [&obj]<typename Info, typename Field>(const Field& field) {
    constexpr std::string_view name = Info::name.AsStringView();
    obj[name] = boost::json::value_from(field);
    if (obj[name].is_null()) {
      if (!T::is_mandatory_field(name)) {
        obj.erase(name);
      } else {
        TGBM_JSON_SERIALIZE_ERROR;
      }
    }
  });
}

template <tgbm::discriminated_api_type T>
inline void tag_invoke(value_from_tag, value& jv, const T& t) {
  boost::json::value obj;

  bool is_empty = true;
  boost::json::object obj_;
  auto m = tgbm::matcher{[&]<typename F>(F& suboneof) {
                           obj = boost::json::object{};
                           auto& o = obj.as_object();
                           o[T::discriminator] = t.discriminator_now();
                           boost::json::value_from(suboneof, obj);
                           is_empty = false;
                         },
                         [](tgbm::nothing_t) {}};
  t.data.visit(m);

  if (is_empty) {
    return;
  }

  jv = std::move(obj);
}

template <tgbm::oneof_field_api_type T>
inline void tag_invoke(value_from_tag, value& jv, const T& t) {
  if (!jv.is_object()) {
    jv = boost::json::object{};
  }
  auto& obj = jv.as_object();
  pfr_extension::visit_object<tgbm::oneof_field_utils::N<T>>(
      t, [&obj]<typename Info, typename Field>(const Field& field) {
        constexpr std::string_view name = Info::name.AsStringView();
        obj[name] = boost::json::value_from(field);
        if (obj[name].is_null()) {
          TGBM_JSON_SERIALIZE_ERROR;
        }
      });

  std::string_view opt_field = t.discriminator_now();
  tgbm::oneof_field_utils::visit(
      t, [&](auto& field) { obj[opt_field] = boost::json::value_from(field); },
      []() { TGBM_JSON_SERIALIZE_ERROR; });
}

}  // namespace boost::json

namespace tgbm::json::boost {
template <aggregate T>
std::string serialize_dom(const T& t) {
  ::boost::json::object j = ::boost::json::value_from(t);
  return ::boost::json::serialize(j);
}
}  // namespace tgbm::json::boost
