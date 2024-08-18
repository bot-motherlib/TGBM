
#ifndef API_TYPE_PATH
#error API_TYPE_PATH should be defined
#endif

namespace tgbm::api {

#define API_TYPE(T) struct T
#include API_TYPE_PATH
{
  // required fields
#define FIELD(Type, Name) Type Name;
#define REQUIRED(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#define COMPOUND(Type, Name) box<Type> Name;
#include API_TYPE_PATH

// optional fields
#define FIELD(Type, Name, ...) Type Name;
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

  static std::optional<bool> consteval is_optional_field(std::string_view name) {
    return utils::string_switch<std::optional<bool>>(name)
#define FIELD(Type, Name) #Name
#define REQUIRED(TYPEMACRO, ...) .case_(TYPEMACRO(__VA_ARGS__), true)
#define OPTIONAL(TYPEMACRO, ...) .case_(TYPEMACRO(__VA_ARGS__), false)
#include API_TYPE_PATH
        .or_default(std::nullopt);
  }

#define API_TYPE(T) std::strong_ordering operator<=>(const T& t) const;
#include API_TYPE_PATH

#define API_TYPE(T) bool operator==(const T& t) const;
#include API_TYPE_PATH
};

}  // namespace tgbm::api
#undef API_TYPE
#undef API_TYPE_PATH
