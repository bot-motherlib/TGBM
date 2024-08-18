
#ifndef API_TYPE_PATH
#error API_TYPE_PATH should be defined
#endif

namespace tgbm::api {

// get only API_TYPE
#define API_TYPE(T) struct T
#include API_TYPE_PATH
{
#include API_TYPE_PATH
  oneof<nothing_t
#define FIELD(Type, Name) , Type
#define VARIANT_FIELD(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#include API_TYPE_PATH
        >
      data;

#define DISCRIMINATOR_FIELD(Name) static constexpr std::string_view discriminator = #Name;

#include API_TYPE_PATH

  enum struct type_e {
    nothing
#define DISCRIMINATOR_VALUE(Type, Name) , k_##Name

#include API_TYPE_PATH
  };

// get variant fields
#define FIELD(Type, Name)                             \
  constexpr Type* get_##Name() noexcept {             \
    return data.get_if<Type>();                       \
  }                                                   \
  constexpr const Type* get_##Name() const noexcept { \
    return data.get_if<Type>();                       \
  }
#define VARIANT_FIELD(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#include API_TYPE_PATH

  static constexpr type_e discriminate(std::string_view val) {
    return utils::string_switch<type_e>(val)
#define DISCRIMINATOR_VALUE(Type, Name) .case_(#Name, type_e::k_##Name)
#include API_TYPE_PATH
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visiter) {
#define DISCRIMINATOR_VALUE(Type, Name) \
  if (val == #Name)                     \
    return visiter.template operator()<Type>();
#include API_TYPE_PATH
    return visiter.template operator()<void>();
  }

  constexpr type_e type() const {
    return static_cast<type_e>(data.index());
  }

#define API_TYPE(T) std::strong_ordering operator<=>(const T& t) const;
#include API_TYPE_PATH

#define API_TYPE(T) bool operator==(const T& t) const;
#include API_TYPE_PATH
};

#define API_TYPE(T) inline std::string_view to_string_view(T::type_e e) {
#include API_TYPE_PATH

#define API_TYPE(T) using From = T::type_e;
#include API_TYPE_PATH
switch (e) {
#define DISCRIMINATOR_VALUE(Type, Name) \
  case From::k_##Name:                  \
    return #Name;
#include API_TYPE_PATH
  case From::nothing:
    return "nothing";
}
}  // namespace tgbm::api

}  // namespace tgbm::api
#undef API_TYPE
#undef API_TYPE_PATH
