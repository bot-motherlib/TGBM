#pragma once

#include <boost/json.hpp>
#include <tgbm/tools/box.hpp>
#include <tgbm/tools/pfr_extension.hpp>
#include <tgbm/tools/traits.hpp>

namespace boost::json {

template <typename T>
void tag_invoke(value_from_tag, value& jv, const std::optional<T>& val) {
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

inline void tag_invoke(value_from_tag, value& jv, std::true_type val) {
  jv = boost::json::value_from(true);
}

}  // namespace boost::json

namespace tgbm {
template <aggregate T>
std::string to_boost_json(const T& t) {
  boost::json::object j;
  pfr_extension::visit_object(t, [&]<typename Info, typename Field>(const Field& field) {
    std::string_view name_field = Info::name.AsStringView();
    j[name_field] = boost::json::value_from(field);
  });
  return boost::json::serialize(j);
}
}  // namespace tgbm
