
#ifndef API_TYPE_PATH
  #error API_TYPE_PATH should be defined
#endif

#ifndef API_TYPE
  #error API_TYPE should be defined
#endif

namespace tgbm::api {

struct API_TYPE {
  // required fields
#define FIELD(Type, Name) Type Name;
#define REQUIRED(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#define COMPOUND(Type, Name) box<Type> Name;
#include API_TYPE_PATH

// optional fields
#define FIELD(Type, Name, ...) optional<Type> Name;
#define BOOLEAN(...)
#define TRUE_FIELD(...)
#define COMPOUND(Type, Name) box<Type> Name;
#define INLINED(Type, Name) Type Name;
#define OPTIONAL(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#include API_TYPE_PATH

// optional boolean flags
#define BOOLEAN(Name, ...) optional<bool> Name;
#define TRUE_FIELD(Name, ...) optional<True> Name;
#define OPTIONAL(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#include API_TYPE_PATH

  consteval static bool is_optional_field(std::string_view name) {
    return utils::string_switch<std::optional<bool>>(name)
#define FIELD(Type, Name) #Name
#define REQUIRED(TYPEMACRO, ...) .case_(TYPEMACRO(__VA_ARGS__), false)
#define OPTIONAL(TYPEMACRO, ...) .case_(TYPEMACRO(__VA_ARGS__), true)
#include API_TYPE_PATH
        .or_default(std::nullopt)
        .value();
  }

  std::strong_ordering operator<=>(const API_TYPE& t) const;

  bool operator==(const API_TYPE& t) const;
};

}  // namespace tgbm::api
#undef API_TYPE_PATH
