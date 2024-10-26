
#ifndef API_TYPE_PATH
  #error API_TYPE_PATH should be defined
#endif

#ifndef API_TYPE
  #error API_TYPE should be defined
#endif

namespace tgbm::api {

struct API_TYPE {
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
#define DISCRIMINATOR_VALUE(Type, Name) k_##Name,
#include API_TYPE_PATH
    nothing
  };
  static constexpr size_t variant_size = size_t(type_e::nothing);

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

  std::strong_ordering operator<=>(const API_TYPE& t) const;

  bool operator==(const API_TYPE& t) const;
};

inline std::string_view to_string_view(API_TYPE::type_e e) {
  using From = API_TYPE::type_e;
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
#undef API_TYPE_PATH
