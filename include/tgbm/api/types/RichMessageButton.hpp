#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a button in a RichMessage. Exactly one of the fields other than text and style must
 * be used to specify the type of the button.*/
struct RichMessageButton {
  /* Text of the button. May contain only plain text, RichTextCustomEmoji and RichTextDateTime entities. */
  box<RichText> text;
  /* Optional. Style of the button. Must be one of “danger”, “success”, “primary”, or “link” (the button is
   * shown as a regular link without borders). Apps may use theme-specific colors for the button background
   * and text based on the style. The style “link” is allowed only for callback buttons. */
  optional<String> style;
  /* Optional. HTTP or tg:// URL to be opened when the button is pressed. Links tg://user?id=<user_id> can be
   * used to mention a user by their identifier without using a username, if this is allowed by their privacy
   * settings. */
  optional<String> url;
  /* Optional. Data to be sent in a callback query to the bot when the button is pressed, 1-64 bytes */
  optional<String> callback_data;
  /* Optional. Description of the Web App that will be launched when the user presses the button. The Web App
   * will be able to send an arbitrary message on behalf of the user using the method answerWebAppQuery.
   * Available only in private chats between a user and the bot. Not supported for messages sent on behalf of
   * a business account. */
  box<WebAppInfo> web_app;
  /* Optional. An HTTPS URL used to automatically authorize the user. Can be used as a replacement for the
   * Telegram Login Widget. Not supported for ephemeral messages. */
  box<LoginUrl> login_url;
  /* Optional. If set, pressing the button will prompt the user to select one of their chats, open that chat
   * and insert the bot's username and the specified inline query in the input field. May be empty, in which
   * case just the bot's username will be inserted. Not supported for messages sent in channel direct messages
   * chats and on behalf of a business account. */
  optional<String> switch_inline_query;
  /* Optional. If set, pressing the button will insert the bot's username and the specified inline query in
   * the current chat's input field. May be empty, in which case only the bot's username will be inserted. Not
   * supported in channels and for messages sent in channel direct messages chats and on behalf of a business
   * account. */
  optional<String> switch_inline_query_current_chat;
  /* Optional. If set, pressing the button will prompt the user to select one of their chats of the specified
   * type, open that chat and insert the bot's username and the specified inline query in the input field. Not
   * supported for messages sent in channel direct messages chats and on behalf of a business account. */
  box<SwitchInlineQueryChosenChat> switch_inline_query_chosen_chat;
  /* Optional. A button that copies the specified text to the clipboard */
  box<CopyTextButton> copy_text;
  /* Optional. If set, then the button is disabled and does nothing */
  box<DisabledButton> disabled;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).or_default(false);
  }
};

}  // namespace tgbm::api
