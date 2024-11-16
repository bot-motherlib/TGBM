#pragma once

#include <tgbm/api/types/all_fwd.hpp>
#include <tgbm/api/types/MenuButtonCommands.hpp>
#include <tgbm/api/types/MenuButtonWebApp.hpp>
#include <tgbm/api/types/MenuButtonDefault.hpp>

namespace tgbm::api {

/*This object describes the bot's menu button in a private chat. It should be one of*/
struct MenuButton {
  oneof<MenuButtonCommands, MenuButtonWebApp, MenuButtonDefault> data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_commands,
    k_webapp,
    k_default,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  MenuButtonCommands* get_commands() noexcept {
    return data.get_if<MenuButtonCommands>();
  }
  const MenuButtonCommands* get_commands() const noexcept {
    return data.get_if<MenuButtonCommands>();
  }
  MenuButtonWebApp* get_webapp() noexcept {
    return data.get_if<MenuButtonWebApp>();
  }
  const MenuButtonWebApp* get_webapp() const noexcept {
    return data.get_if<MenuButtonWebApp>();
  }
  MenuButtonDefault* get_default() noexcept {
    return data.get_if<MenuButtonDefault>();
  }
  const MenuButtonDefault* get_default() const noexcept {
    return data.get_if<MenuButtonDefault>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return string_switch<type_e>(val)
        .case_("commands", type_e::k_commands)
        .case_("web_app", type_e::k_webapp)
        .case_("default", type_e::k_default)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "commands")
      return visitor.template operator()<MenuButtonCommands>();
    if (val == "web_app")
      return visitor.template operator()<MenuButtonWebApp>();
    if (val == "default")
      return visitor.template operator()<MenuButtonDefault>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum MenuButton::type_e;
    switch (type()) {
      case k_commands:
        return "commands";
      case k_webapp:
        return "web_app";
      case k_default:
        return "default";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
