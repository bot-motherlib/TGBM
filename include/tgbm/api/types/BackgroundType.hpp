#pragma once

#include "all_fwd.hpp"
#include "BackgroundTypeFill.hpp"
#include "BackgroundTypeWallpaper.hpp"
#include "BackgroundTypePattern.hpp"
#include "BackgroundTypeChatTheme.hpp"

namespace tgbm::api {

/*This object describes the type of a background. Currently, it can be one of*/
struct BackgroundType {
  oneof<BackgroundTypeFill, BackgroundTypeWallpaper, BackgroundTypePattern, BackgroundTypeChatTheme> data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_fill,
    k_wallpaper,
    k_pattern,
    k_chattheme,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  BackgroundTypeFill* get_fill() noexcept {
    return data.get_if<BackgroundTypeFill>();
  }
  const BackgroundTypeFill* get_fill() const noexcept {
    return data.get_if<BackgroundTypeFill>();
  }
  BackgroundTypeWallpaper* get_wallpaper() noexcept {
    return data.get_if<BackgroundTypeWallpaper>();
  }
  const BackgroundTypeWallpaper* get_wallpaper() const noexcept {
    return data.get_if<BackgroundTypeWallpaper>();
  }
  BackgroundTypePattern* get_pattern() noexcept {
    return data.get_if<BackgroundTypePattern>();
  }
  const BackgroundTypePattern* get_pattern() const noexcept {
    return data.get_if<BackgroundTypePattern>();
  }
  BackgroundTypeChatTheme* get_chattheme() noexcept {
    return data.get_if<BackgroundTypeChatTheme>();
  }
  const BackgroundTypeChatTheme* get_chattheme() const noexcept {
    return data.get_if<BackgroundTypeChatTheme>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return string_switch<type_e>(val)
        .case_("fill", type_e::k_fill)
        .case_("wallpaper", type_e::k_wallpaper)
        .case_("pattern", type_e::k_pattern)
        .case_("chat_theme", type_e::k_chattheme)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "fill")
      return visitor.template operator()<BackgroundTypeFill>();
    if (val == "wallpaper")
      return visitor.template operator()<BackgroundTypeWallpaper>();
    if (val == "pattern")
      return visitor.template operator()<BackgroundTypePattern>();
    if (val == "chat_theme")
      return visitor.template operator()<BackgroundTypeChatTheme>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum BackgroundType::type_e;
    switch (type()) {
      case k_fill:
        return "fill";
      case k_wallpaper:
        return "wallpaper";
      case k_pattern:
        return "pattern";
      case k_chattheme:
        return "chat_theme";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
