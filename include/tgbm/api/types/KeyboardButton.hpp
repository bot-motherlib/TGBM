#pragma once

#include <tgbm/api/types/KeyboardButtonPollType.hpp>
#include <tgbm/api/types/KeyboardButtonRequestChat.hpp>
#include <tgbm/api/types/KeyboardButtonRequestManagedBot.hpp>
#include <tgbm/api/types/KeyboardButtonRequestUsers.hpp>
#include <tgbm/api/types/WebAppInfo.hpp>
#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

struct KeyboardButton {
  /* Text of the button. If none of the fields other than text, icon_custom_emoji_id, and style are used, it
   * will be sent as a message when the button is pressed */
  String text;
  struct icon_custom_emoji_id {
    String value;
  };
  struct style {
    String value;
  };
  struct request_users {
    KeyboardButtonRequestUsers value;
  };
  struct request_chat {
    KeyboardButtonRequestChat value;
  };
  struct request_managed_bot {
    KeyboardButtonRequestManagedBot value;
  };
  struct request_poll {
    KeyboardButtonPollType value;
  };
  struct web_app {
    WebAppInfo value;
  };
  struct request_contact {
    bool value;
  };
  struct request_location {
    bool value;
  };
  oneof<icon_custom_emoji_id, style, request_users, request_chat, request_managed_bot, request_poll, web_app,
        request_contact, request_location>
      data;
  enum struct type_e {
    /* Optional. Unique identifier of the custom emoji shown before the text of the button. Can only be used
       by bots that purchased additional usernames on Fragment or in the messages directly sent by the bot to
       private, group and supergroup chats if the owner of the bot has a Telegram Premium subscription. */
    k_icon_custom_emoji_id,
    /* Optional. Style of the button. Must be one of “danger” (red), “success” (green) or “primary” (blue). If
       omitted, then an app-specific style is used. */
    k_style,
    /* Optional. If specified, pressing the button will open a list of suitable users. Identifiers of selected
       users will be sent to the bot in a “users_shared” service message. Available in private chats only. */
    k_request_users,
    /* Optional. If specified, pressing the button will open a list of suitable chats. Tapping on a chat will
       send its identifier to the bot in a “chat_shared” service message. Available in private chats only. */
    k_request_chat,
    /* Optional. If specified, pressing the button will ask the user to create and share a bot that will be
       managed by the current bot. Available for bots that enabled management of other bots in the @BotFather
       Mini App. Available in private chats only. */
    k_request_managed_bot,
    /* Optional. If specified, the user will be asked to create a poll and send it to the bot when the button
       is pressed. Available in private chats only. */
    k_request_poll,
    /* Optional. If specified, the described Web App will be launched when the button is pressed. The Web App
       will be able to send a “web_app_data” service message. Available in private chats only. */
    k_web_app,
    /* Optional. If True, the user's phone number will be sent as a contact when the button is pressed.
       Available in private chats only. */
    k_request_contact,
    /* Optional. If True, the user's current location will be sent when the button is pressed. Available in
       private chats only. */
    k_request_location,
    nothing,
  };
  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  String* get_icon_custom_emoji_id() noexcept {
    auto* p = data.get_if<icon_custom_emoji_id>();
    return p ? &p->value : nullptr;
  }
  const String* get_icon_custom_emoji_id() const noexcept {
    auto* p = data.get_if<icon_custom_emoji_id>();
    return p ? &p->value : nullptr;
  }
  String* get_style() noexcept {
    auto* p = data.get_if<style>();
    return p ? &p->value : nullptr;
  }
  const String* get_style() const noexcept {
    auto* p = data.get_if<style>();
    return p ? &p->value : nullptr;
  }
  KeyboardButtonRequestUsers* get_request_users() noexcept {
    auto* p = data.get_if<request_users>();
    return p ? &p->value : nullptr;
  }
  const KeyboardButtonRequestUsers* get_request_users() const noexcept {
    auto* p = data.get_if<request_users>();
    return p ? &p->value : nullptr;
  }
  KeyboardButtonRequestChat* get_request_chat() noexcept {
    auto* p = data.get_if<request_chat>();
    return p ? &p->value : nullptr;
  }
  const KeyboardButtonRequestChat* get_request_chat() const noexcept {
    auto* p = data.get_if<request_chat>();
    return p ? &p->value : nullptr;
  }
  KeyboardButtonRequestManagedBot* get_request_managed_bot() noexcept {
    auto* p = data.get_if<request_managed_bot>();
    return p ? &p->value : nullptr;
  }
  const KeyboardButtonRequestManagedBot* get_request_managed_bot() const noexcept {
    auto* p = data.get_if<request_managed_bot>();
    return p ? &p->value : nullptr;
  }
  KeyboardButtonPollType* get_request_poll() noexcept {
    auto* p = data.get_if<request_poll>();
    return p ? &p->value : nullptr;
  }
  const KeyboardButtonPollType* get_request_poll() const noexcept {
    auto* p = data.get_if<request_poll>();
    return p ? &p->value : nullptr;
  }
  WebAppInfo* get_web_app() noexcept {
    auto* p = data.get_if<web_app>();
    return p ? &p->value : nullptr;
  }
  const WebAppInfo* get_web_app() const noexcept {
    auto* p = data.get_if<web_app>();
    return p ? &p->value : nullptr;
  }
  bool* get_request_contact() noexcept {
    auto* p = data.get_if<request_contact>();
    return p ? &p->value : nullptr;
  }
  const bool* get_request_contact() const noexcept {
    auto* p = data.get_if<request_contact>();
    return p ? &p->value : nullptr;
  }
  bool* get_request_location() noexcept {
    auto* p = data.get_if<request_location>();
    return p ? &p->value : nullptr;
  }
  const bool* get_request_location() const noexcept {
    auto* p = data.get_if<request_location>();
    return p ? &p->value : nullptr;
  }
  static constexpr decltype(auto) discriminate_field(std::string_view val, auto&& visitor) {
    if (val == "icon_custom_emoji_id")
      return visitor.template operator()<icon_custom_emoji_id>();
    if (val == "style")
      return visitor.template operator()<style>();
    if (val == "request_users")
      return visitor.template operator()<request_users>();
    if (val == "request_chat")
      return visitor.template operator()<request_chat>();
    if (val == "request_managed_bot")
      return visitor.template operator()<request_managed_bot>();
    if (val == "request_poll")
      return visitor.template operator()<request_poll>();
    if (val == "web_app")
      return visitor.template operator()<web_app>();
    if (val == "request_contact")
      return visitor.template operator()<request_contact>();
    if (val == "request_location")
      return visitor.template operator()<request_location>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum KeyboardButton::type_e;
    switch (type()) {
      case k_icon_custom_emoji_id:
        return "icon_custom_emoji_id";
      case k_style:
        return "style";
      case k_request_users:
        return "request_users";
      case k_request_chat:
        return "request_chat";
      case k_request_managed_bot:
        return "request_managed_bot";
      case k_request_poll:
        return "request_poll";
      case k_web_app:
        return "web_app";
      case k_request_contact:
        return "request_contact";
      case k_request_location:
        return "request_location";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
