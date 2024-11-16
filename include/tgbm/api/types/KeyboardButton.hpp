#pragma once

#include <tgbm/api/types/KeyboardButtonPollType.hpp>
#include <tgbm/api/types/KeyboardButtonRequestChat.hpp>
#include <tgbm/api/types/KeyboardButtonRequestUsers.hpp>
#include <tgbm/api/types/WebAppInfo.hpp>
#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

struct KeyboardButton {
  /* Text of the button. If none of the optional fields are used, it will be sent as a message when the button
   * is pressed */
  String text;
  struct request_users {
    KeyboardButtonRequestUsers value;
  };
  struct request_chat {
    KeyboardButtonRequestChat value;
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
  oneof<request_users, request_chat, request_poll, web_app, request_contact, request_location> data;
  enum struct type_e {
    /* Optional. If specified, pressing the button will open a list of suitable users. Identifiers of selected
       users will be sent to the bot in a “users_shared” service message. Available in private chats only. */
    k_request_users,
    /* Optional. If specified, pressing the button will open a list of suitable chats. Tapping on a chat will
       send its identifier to the bot in a “chat_shared” service message. Available in private chats only. */
    k_request_chat,
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
    if (val == "request_users")
      return visitor.template operator()<request_users>();
    if (val == "request_chat")
      return visitor.template operator()<request_chat>();
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
      case k_request_users:
        return "request_users";
      case k_request_chat:
        return "request_chat";
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
