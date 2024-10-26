
#ifndef API_TYPE_PATH
  #error API_TYPE_PATH should be defined
#endif

#ifndef API_TYPE
  #error API_TYPE should be defined
#endif

namespace tgbm::api {

#define CONCAT_FIELDS(A) A##_fields
#define CONCAT_FIELDS1(A) CONCAT_FIELDS(A)
#define API_FIELDS CONCAT_FIELDS1(API_TYPE)

struct API_TYPE {
#define FIELD(Type, Name, ...) \
  struct Name {                \
    Type value;                \
  };
#define OPTIONAL(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#include API_TYPE_PATH
  // required fields
#define FIELD(Type, Name) Type Name;
#define REQUIRED(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#define COMPOUND(Type, Name) box<Type> Name;
#include API_TYPE_PATH

  // optional fields
  oneof<placeholder
#define FIELD(Type, Name, ...) , Name
#define OPTIONAL(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#include API_TYPE_PATH
        >
      data;

  enum struct type_e {
#define FIELD(Type, Name, ...) k_##Name,
#define OPTIONAL(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#include API_TYPE_PATH
    nothing
  };
  static constexpr size_t variant_size = size_t(type_e::nothing);

// get variant fields
#define FIELD(Type, Name)                             \
  constexpr Name* get_##Name() noexcept {             \
    return data.get_if<Name>();                       \
  }                                                   \
  constexpr const Name* get_##Name() const noexcept { \
    return data.get_if<Name>();                       \
  }
#define OPTIONAL(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#include API_TYPE_PATH

  static constexpr decltype(auto) discriminate_field(std::string_view val, auto&& visiter) {
#define OPTIONAL(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#define FIELD(Type, Name, ...) \
  if (val == #Name)            \
    return visiter.template operator()<Name>();
#include API_TYPE_PATH
    return visiter.template operator()<void>();
  }

  constexpr type_e type() const {
    return static_cast<type_e>(data.index());
  }
};

inline std::string_view to_string_view(API_TYPE::type_e e) {
  using From = API_TYPE::type_e;
  switch (e) {
#define OPTIONAL(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#define FIELD(Type, Name, ...) \
  case From::k_##Name:         \
    return #Name;
#include API_TYPE_PATH
    case From::nothing:
      return "nothing";
  }
}

namespace API_FIELDS {
#define FIELD(Type, Name, ...) using Name = API_TYPE::Name;
#define OPTIONAL(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#include API_TYPE_PATH

}  // namespace API_FIELDS

}  // namespace tgbm::api
#undef API_FIELDS
#undef API_TYPE_PATH
