#pragma once

#include "WebAppInfo.hpp"
#include "CopyTextButton.hpp"
#include "SwitchInlineQueryChosenChat.hpp"
#include "CallbackGame.hpp"
#include "all_fwd.hpp"
#include "LoginUrl.hpp"

namespace tgbm::api {

struct InlineKeyboardButton {
  /* Label text on the button */
  String text;
  struct url {
    String value;
  };
  struct callback_data {
    String value;
  };
  struct web_app {
    WebAppInfo value;
  };
  struct login_url {
    LoginUrl value;
  };
  struct switch_inline_query {
    String value;
  };
  struct switch_inline_query_current_chat {
    String value;
  };
  struct switch_inline_query_chosen_chat {
    SwitchInlineQueryChosenChat value;
  };
  struct copy_text {
    CopyTextButton value;
  };
  struct callback_game {
    CallbackGame value;
  };
  struct pay {
    bool value;
  };
  oneof<url, callback_data, web_app, login_url, switch_inline_query, switch_inline_query_current_chat,
        switch_inline_query_chosen_chat, copy_text, callback_game, pay>
      data;
  enum struct type_e {
    /* Optional. HTTP or tg:// URL to be opened when the button is pressed. Links tg://user?id=<user_id> can
       be used to mention a user by their identifier without using a username, if this is allowed by their
       privacy settings. */
    k_url,
    /* Optional. Data to be sent in a callback query to the bot when the button is pressed, 1-64 bytes */
    k_callback_data,
    /* Optional. Description of the Web App that will be launched when the user presses the button. The Web
       App will be able to send an arbitrary message on behalf of the user using the method answerWebAppQuery.
       Available only in private chats between a user and the bot. Not supported for messages sent on behalf
       of a Telegram Business account. */
    k_web_app,
    /* Optional. An HTTPS URL used to automatically authorize the user. Can be used as a replacement for the
       Telegram Login Widget. */
    k_login_url,
    /* Optional. If set, pressing the button will prompt the user to select one of their chats, open that chat
       and insert the bot's username and the specified inline query in the input field. May be empty, in which
       case just the bot's username will be inserted. Not supported for messages sent on behalf of a Telegram
       Business account. */
    k_switch_inline_query,
    /* Optional. If set, pressing the button will insert the bot's username and the specified inline query in
       the current chat's input field. May be empty, in which case only the bot's username will be
       inserted.This offers a quick way for the user to open your bot in inline mode in the same chat - good
       for selecting something from multiple options. Not supported in channels and for messages sent on
       behalf of a Telegram Business account. */
    k_switch_inline_query_current_chat,
    /* Optional. If set, pressing the button will prompt the user to select one of their chats of the
       specified type, open that chat and insert the bot's username and the specified inline query in the
       input field. Not supported for messages sent on behalf of a Telegram Business account. */
    k_switch_inline_query_chosen_chat,
    /* Optional. Description of the button that copies the specified text to the clipboard. */
    k_copy_text,
    /* Optional. Description of the game that will be launched when the user presses the button.NOTE: This
       type of button must always be the first button in the first row. */
    k_callback_game,
    /* Optional. Specify True, to send a Pay button. Substrings “” and “XTR” in the buttons's text will be
       replaced with a Telegram Star icon.NOTE: This type of button must always be the first button in the
       first row and can only be used in invoice messages. */
    k_pay,
    nothing,
  };
  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  String* get_url() noexcept {
    auto* p = data.get_if<url>();
    return p ? &p->value : nullptr;
  }
  const String* get_url() const noexcept {
    auto* p = data.get_if<url>();
    return p ? &p->value : nullptr;
  }
  String* get_callback_data() noexcept {
    auto* p = data.get_if<callback_data>();
    return p ? &p->value : nullptr;
  }
  const String* get_callback_data() const noexcept {
    auto* p = data.get_if<callback_data>();
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
  LoginUrl* get_login_url() noexcept {
    auto* p = data.get_if<login_url>();
    return p ? &p->value : nullptr;
  }
  const LoginUrl* get_login_url() const noexcept {
    auto* p = data.get_if<login_url>();
    return p ? &p->value : nullptr;
  }
  String* get_switch_inline_query() noexcept {
    auto* p = data.get_if<switch_inline_query>();
    return p ? &p->value : nullptr;
  }
  const String* get_switch_inline_query() const noexcept {
    auto* p = data.get_if<switch_inline_query>();
    return p ? &p->value : nullptr;
  }
  String* get_switch_inline_query_current_chat() noexcept {
    auto* p = data.get_if<switch_inline_query_current_chat>();
    return p ? &p->value : nullptr;
  }
  const String* get_switch_inline_query_current_chat() const noexcept {
    auto* p = data.get_if<switch_inline_query_current_chat>();
    return p ? &p->value : nullptr;
  }
  SwitchInlineQueryChosenChat* get_switch_inline_query_chosen_chat() noexcept {
    auto* p = data.get_if<switch_inline_query_chosen_chat>();
    return p ? &p->value : nullptr;
  }
  const SwitchInlineQueryChosenChat* get_switch_inline_query_chosen_chat() const noexcept {
    auto* p = data.get_if<switch_inline_query_chosen_chat>();
    return p ? &p->value : nullptr;
  }
  CopyTextButton* get_copy_text() noexcept {
    auto* p = data.get_if<copy_text>();
    return p ? &p->value : nullptr;
  }
  const CopyTextButton* get_copy_text() const noexcept {
    auto* p = data.get_if<copy_text>();
    return p ? &p->value : nullptr;
  }
  CallbackGame* get_callback_game() noexcept {
    auto* p = data.get_if<callback_game>();
    return p ? &p->value : nullptr;
  }
  const CallbackGame* get_callback_game() const noexcept {
    auto* p = data.get_if<callback_game>();
    return p ? &p->value : nullptr;
  }
  bool* get_pay() noexcept {
    auto* p = data.get_if<pay>();
    return p ? &p->value : nullptr;
  }
  const bool* get_pay() const noexcept {
    auto* p = data.get_if<pay>();
    return p ? &p->value : nullptr;
  }
  static constexpr decltype(auto) discriminate_field(std::string_view val, auto&& visitor) {
    if (val == "url")
      return visitor.template operator()<url>();
    if (val == "callback_data")
      return visitor.template operator()<callback_data>();
    if (val == "web_app")
      return visitor.template operator()<web_app>();
    if (val == "login_url")
      return visitor.template operator()<login_url>();
    if (val == "switch_inline_query")
      return visitor.template operator()<switch_inline_query>();
    if (val == "switch_inline_query_current_chat")
      return visitor.template operator()<switch_inline_query_current_chat>();
    if (val == "switch_inline_query_chosen_chat")
      return visitor.template operator()<switch_inline_query_chosen_chat>();
    if (val == "copy_text")
      return visitor.template operator()<copy_text>();
    if (val == "callback_game")
      return visitor.template operator()<callback_game>();
    if (val == "pay")
      return visitor.template operator()<pay>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum InlineKeyboardButton::type_e;
    switch (type()) {
      case k_url:
        return "url";
      case k_callback_data:
        return "callback_data";
      case k_web_app:
        return "web_app";
      case k_login_url:
        return "login_url";
      case k_switch_inline_query:
        return "switch_inline_query";
      case k_switch_inline_query_current_chat:
        return "switch_inline_query_current_chat";
      case k_switch_inline_query_chosen_chat:
        return "switch_inline_query_chosen_chat";
      case k_copy_text:
        return "copy_text";
      case k_callback_game:
        return "callback_game";
      case k_pay:
        return "pay";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
