#pragma once

#include <tgbm/api/types/WebAppInfo.hpp>
#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

struct InlineQueryResultsButton {
  /* Label text on the button */
  String text;
  struct web_app {
    WebAppInfo value;
  };
  struct start_parameter {
    String value;
  };
  oneof<web_app, start_parameter> data;
  enum struct type_e {
    /* Optional. Description of the Web App that will be launched when the user presses the button. The Web
       App will be able to switch back to the inline mode using the method switchInlineQuery inside the Web
       App. */
    k_web_app,
    /* Optional. Deep-linking parameter for the /start message sent to the bot when a user presses the button.
       1-64 characters, only A-Z, a-z, 0-9, _ and - are allowed.Example: An inline bot that sends YouTube
       videos can ask the user to connect the bot to their YouTube account to adapt search results
       accordingly. To do this, it displays a 'Connect your YouTube account' button above the results, or even
       before showing any. The user presses the button, switches to a private chat with the bot and, in doing
       so, passes a start parameter that instructs the bot to return an OAuth link. Once done, the bot can
       offer a switch_inline button so that the user can easily return to the chat where they wanted to use
       the bot's inline capabilities. */
    k_start_parameter,
    nothing,
  };
  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  WebAppInfo* get_web_app() noexcept {
    auto* p = data.get_if<web_app>();
    return p ? &p->value : nullptr;
  }
  const WebAppInfo* get_web_app() const noexcept {
    auto* p = data.get_if<web_app>();
    return p ? &p->value : nullptr;
  }
  String* get_start_parameter() noexcept {
    auto* p = data.get_if<start_parameter>();
    return p ? &p->value : nullptr;
  }
  const String* get_start_parameter() const noexcept {
    auto* p = data.get_if<start_parameter>();
    return p ? &p->value : nullptr;
  }
  static constexpr decltype(auto) discriminate_field(std::string_view val, auto&& visitor) {
    if (val == "web_app")
      return visitor.template operator()<web_app>();
    if (val == "start_parameter")
      return visitor.template operator()<start_parameter>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum InlineQueryResultsButton::type_e;
    switch (type()) {
      case k_web_app:
        return "web_app";
      case k_start_parameter:
        return "start_parameter";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
