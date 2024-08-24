
#ifndef API_TYPE_PATH
#error API_TYPE_PATH should be defined
#endif

#include "tgbm/api_types/common.h"

namespace tgbm::api {

struct
// get only API_TYPE
#define API_TYPE(T) T
#include API_TYPE_PATH
{
  // mandatory fields
#define FIELD(Type, Name) Type Name;
#define REQUIRED(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#include API_TYPE_PATH

// optional fields
#define FIELD(Type, Name, ...) Type Name;
#define BOOLEAN(...)
#define TRUE_FIELD(...)
#define OPTIONAL(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#include API_TYPE_PATH

// optional boolean flags
#define BOOLEAN(Name, ...) <bool> Name;
#define TRUE_FIELD(Name, ...) optional<True> Name;
#define OPTIONAL(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#include API_TYPE_PATH

  static consteval bool is_optional_field(std::string_view name) {
    return utils::string_switch<bool>(name)
#define FIELD(Type, Name) .case_(#Name, true)
#include API_TYPE_PATH
        .or_default(false);
  }
};

}  // namespace tgbm::api
#undef API_TYPE
#undef API_TYPE_PATH
