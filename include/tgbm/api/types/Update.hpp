#pragma once

#include "BusinessConnection.hpp"
#include "BusinessMessagesDeleted.hpp"
#include "CallbackQuery.hpp"
#include "ChatBoostRemoved.hpp"
#include "ChatBoostUpdated.hpp"
#include "ChatJoinRequest.hpp"
#include "ChatMemberUpdated.hpp"
#include "ChosenInlineResult.hpp"
#include "InlineQuery.hpp"
#include "Message.hpp"
#include "MessageReactionCountUpdated.hpp"
#include "MessageReactionUpdated.hpp"
#include "PaidMediaPurchased.hpp"
#include "Poll.hpp"
#include "PollAnswer.hpp"
#include "PreCheckoutQuery.hpp"
#include "ShippingQuery.hpp"
#include "all_fwd.hpp"

namespace tgbm::api {

struct Update {
  /* The update's unique identifier. Update identifiers start from a certain positive number and increase
   * sequentially. This identifier becomes especially handy if you're using webhooks, since it allows you to
   * ignore repeated updates or to restore the correct update sequence, should they get out of order. If there
   * are no new updates for at least a week, then identifier of the next update will be chosen randomly
   * instead of sequentially. */
  Integer update_id;
  struct message {
    Message value;
  };
  struct edited_message {
    Message value;
  };
  struct channel_post {
    Message value;
  };
  struct edited_channel_post {
    Message value;
  };
  struct business_connection {
    BusinessConnection value;
  };
  struct business_message {
    Message value;
  };
  struct edited_business_message {
    Message value;
  };
  struct deleted_business_messages {
    BusinessMessagesDeleted value;
  };
  struct message_reaction {
    MessageReactionUpdated value;
  };
  struct message_reaction_count {
    MessageReactionCountUpdated value;
  };
  struct inline_query {
    InlineQuery value;
  };
  struct chosen_inline_result {
    ChosenInlineResult value;
  };
  struct callback_query {
    CallbackQuery value;
  };
  struct shipping_query {
    ShippingQuery value;
  };
  struct pre_checkout_query {
    PreCheckoutQuery value;
  };
  struct purchased_paid_media {
    PaidMediaPurchased value;
  };
  struct poll {
    Poll value;
  };
  struct poll_answer {
    PollAnswer value;
  };
  struct my_chat_member {
    ChatMemberUpdated value;
  };
  struct chat_member {
    ChatMemberUpdated value;
  };
  struct chat_join_request {
    ChatJoinRequest value;
  };
  struct chat_boost {
    ChatBoostUpdated value;
  };
  struct removed_chat_boost {
    ChatBoostRemoved value;
  };
  oneof<message, edited_message, channel_post, edited_channel_post, business_connection, business_message,
        edited_business_message, deleted_business_messages, message_reaction, message_reaction_count,
        inline_query, chosen_inline_result, callback_query, shipping_query, pre_checkout_query,
        purchased_paid_media, poll, poll_answer, my_chat_member, chat_member, chat_join_request, chat_boost,
        removed_chat_boost>
      data;
  enum struct type_e {
    /* Optional. New incoming message of any kind - text, photo, sticker, etc. */
    k_message,
    /* Optional. New version of a message that is known to the bot and was edited. This update may at times be
       triggered by changes to message fields that are either unavailable or not actively used by your bot. */
    k_edited_message,
    /* Optional. New incoming channel post of any kind - text, photo, sticker, etc. */
    k_channel_post,
    /* Optional. New version of a channel post that is known to the bot and was edited. This update may at
       times be triggered by changes to message fields that are either unavailable or not actively used by
       your bot. */
    k_edited_channel_post,
    /* Optional. The bot was connected to or disconnected from a business account, or a user edited an
       existing connection with the bot */
    k_business_connection,
    /* Optional. New message from a connected business account */
    k_business_message,
    /* Optional. New version of a message from a connected business account */
    k_edited_business_message,
    /* Optional. Messages were deleted from a connected business account */
    k_deleted_business_messages,
    /* Optional. A reaction to a message was changed by a user. The bot must be an administrator in the chat
       and must explicitly specify "message_reaction" in the list of allowed_updates to receive these updates.
       The update isn't received for reactions set by bots. */
    k_message_reaction,
    /* Optional. Reactions to a message with anonymous reactions were changed. The bot must be an
       administrator in the chat and must explicitly specify "message_reaction_count" in the list of
       allowed_updates to receive these updates. The updates are grouped and can be sent with delay up to a
       few minutes. */
    k_message_reaction_count,
    /* Optional. New incoming inline query */
    k_inline_query,
    /* Optional. The result of an inline query that was chosen by a user and sent to their chat partner.
       Please see our documentation on the feedback collecting for details on how to enable these updates for
       your bot. */
    k_chosen_inline_result,
    /* Optional. New incoming callback query */
    k_callback_query,
    /* Optional. New incoming shipping query. Only for invoices with flexible price */
    k_shipping_query,
    /* Optional. New incoming pre-checkout query. Contains full information about checkout */
    k_pre_checkout_query,
    /* Optional. A user purchased paid media with a non-empty payload sent by the bot in a non-channel chat */
    k_purchased_paid_media,
    /* Optional. New poll state. Bots receive only updates about manually stopped polls and polls, which are
       sent by the bot */
    k_poll,
    /* Optional. A user changed their answer in a non-anonymous poll. Bots receive new votes only in polls
       that were sent by the bot itself. */
    k_poll_answer,
    /* Optional. The bot's chat member status was updated in a chat. For private chats, this update is
       received only when the bot is blocked or unblocked by the user. */
    k_my_chat_member,
    /* Optional. A chat member's status was updated in a chat. The bot must be an administrator in the chat
       and must explicitly specify "chat_member" in the list of allowed_updates to receive these updates. */
    k_chat_member,
    /* Optional. A request to join the chat has been sent. The bot must have the can_invite_users
       administrator right in the chat to receive these updates. */
    k_chat_join_request,
    /* Optional. A chat boost was added or changed. The bot must be an administrator in the chat to receive
       these updates. */
    k_chat_boost,
    /* Optional. A boost was removed from a chat. The bot must be an administrator in the chat to receive
       these updates. */
    k_removed_chat_boost,
    nothing,
  };
  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  Message* get_message() noexcept {
    auto* p = data.get_if<message>();
    return p ? &p->value : nullptr;
  }
  const Message* get_message() const noexcept {
    auto* p = data.get_if<message>();
    return p ? &p->value : nullptr;
  }
  Message* get_edited_message() noexcept {
    auto* p = data.get_if<edited_message>();
    return p ? &p->value : nullptr;
  }
  const Message* get_edited_message() const noexcept {
    auto* p = data.get_if<edited_message>();
    return p ? &p->value : nullptr;
  }
  Message* get_channel_post() noexcept {
    auto* p = data.get_if<channel_post>();
    return p ? &p->value : nullptr;
  }
  const Message* get_channel_post() const noexcept {
    auto* p = data.get_if<channel_post>();
    return p ? &p->value : nullptr;
  }
  Message* get_edited_channel_post() noexcept {
    auto* p = data.get_if<edited_channel_post>();
    return p ? &p->value : nullptr;
  }
  const Message* get_edited_channel_post() const noexcept {
    auto* p = data.get_if<edited_channel_post>();
    return p ? &p->value : nullptr;
  }
  BusinessConnection* get_business_connection() noexcept {
    auto* p = data.get_if<business_connection>();
    return p ? &p->value : nullptr;
  }
  const BusinessConnection* get_business_connection() const noexcept {
    auto* p = data.get_if<business_connection>();
    return p ? &p->value : nullptr;
  }
  Message* get_business_message() noexcept {
    auto* p = data.get_if<business_message>();
    return p ? &p->value : nullptr;
  }
  const Message* get_business_message() const noexcept {
    auto* p = data.get_if<business_message>();
    return p ? &p->value : nullptr;
  }
  Message* get_edited_business_message() noexcept {
    auto* p = data.get_if<edited_business_message>();
    return p ? &p->value : nullptr;
  }
  const Message* get_edited_business_message() const noexcept {
    auto* p = data.get_if<edited_business_message>();
    return p ? &p->value : nullptr;
  }
  BusinessMessagesDeleted* get_deleted_business_messages() noexcept {
    auto* p = data.get_if<deleted_business_messages>();
    return p ? &p->value : nullptr;
  }
  const BusinessMessagesDeleted* get_deleted_business_messages() const noexcept {
    auto* p = data.get_if<deleted_business_messages>();
    return p ? &p->value : nullptr;
  }
  MessageReactionUpdated* get_message_reaction() noexcept {
    auto* p = data.get_if<message_reaction>();
    return p ? &p->value : nullptr;
  }
  const MessageReactionUpdated* get_message_reaction() const noexcept {
    auto* p = data.get_if<message_reaction>();
    return p ? &p->value : nullptr;
  }
  MessageReactionCountUpdated* get_message_reaction_count() noexcept {
    auto* p = data.get_if<message_reaction_count>();
    return p ? &p->value : nullptr;
  }
  const MessageReactionCountUpdated* get_message_reaction_count() const noexcept {
    auto* p = data.get_if<message_reaction_count>();
    return p ? &p->value : nullptr;
  }
  InlineQuery* get_inline_query() noexcept {
    auto* p = data.get_if<inline_query>();
    return p ? &p->value : nullptr;
  }
  const InlineQuery* get_inline_query() const noexcept {
    auto* p = data.get_if<inline_query>();
    return p ? &p->value : nullptr;
  }
  ChosenInlineResult* get_chosen_inline_result() noexcept {
    auto* p = data.get_if<chosen_inline_result>();
    return p ? &p->value : nullptr;
  }
  const ChosenInlineResult* get_chosen_inline_result() const noexcept {
    auto* p = data.get_if<chosen_inline_result>();
    return p ? &p->value : nullptr;
  }
  CallbackQuery* get_callback_query() noexcept {
    auto* p = data.get_if<callback_query>();
    return p ? &p->value : nullptr;
  }
  const CallbackQuery* get_callback_query() const noexcept {
    auto* p = data.get_if<callback_query>();
    return p ? &p->value : nullptr;
  }
  ShippingQuery* get_shipping_query() noexcept {
    auto* p = data.get_if<shipping_query>();
    return p ? &p->value : nullptr;
  }
  const ShippingQuery* get_shipping_query() const noexcept {
    auto* p = data.get_if<shipping_query>();
    return p ? &p->value : nullptr;
  }
  PreCheckoutQuery* get_pre_checkout_query() noexcept {
    auto* p = data.get_if<pre_checkout_query>();
    return p ? &p->value : nullptr;
  }
  const PreCheckoutQuery* get_pre_checkout_query() const noexcept {
    auto* p = data.get_if<pre_checkout_query>();
    return p ? &p->value : nullptr;
  }
  PaidMediaPurchased* get_purchased_paid_media() noexcept {
    auto* p = data.get_if<purchased_paid_media>();
    return p ? &p->value : nullptr;
  }
  const PaidMediaPurchased* get_purchased_paid_media() const noexcept {
    auto* p = data.get_if<purchased_paid_media>();
    return p ? &p->value : nullptr;
  }
  Poll* get_poll() noexcept {
    auto* p = data.get_if<poll>();
    return p ? &p->value : nullptr;
  }
  const Poll* get_poll() const noexcept {
    auto* p = data.get_if<poll>();
    return p ? &p->value : nullptr;
  }
  PollAnswer* get_poll_answer() noexcept {
    auto* p = data.get_if<poll_answer>();
    return p ? &p->value : nullptr;
  }
  const PollAnswer* get_poll_answer() const noexcept {
    auto* p = data.get_if<poll_answer>();
    return p ? &p->value : nullptr;
  }
  ChatMemberUpdated* get_my_chat_member() noexcept {
    auto* p = data.get_if<my_chat_member>();
    return p ? &p->value : nullptr;
  }
  const ChatMemberUpdated* get_my_chat_member() const noexcept {
    auto* p = data.get_if<my_chat_member>();
    return p ? &p->value : nullptr;
  }
  ChatMemberUpdated* get_chat_member() noexcept {
    auto* p = data.get_if<chat_member>();
    return p ? &p->value : nullptr;
  }
  const ChatMemberUpdated* get_chat_member() const noexcept {
    auto* p = data.get_if<chat_member>();
    return p ? &p->value : nullptr;
  }
  ChatJoinRequest* get_chat_join_request() noexcept {
    auto* p = data.get_if<chat_join_request>();
    return p ? &p->value : nullptr;
  }
  const ChatJoinRequest* get_chat_join_request() const noexcept {
    auto* p = data.get_if<chat_join_request>();
    return p ? &p->value : nullptr;
  }
  ChatBoostUpdated* get_chat_boost() noexcept {
    auto* p = data.get_if<chat_boost>();
    return p ? &p->value : nullptr;
  }
  const ChatBoostUpdated* get_chat_boost() const noexcept {
    auto* p = data.get_if<chat_boost>();
    return p ? &p->value : nullptr;
  }
  ChatBoostRemoved* get_removed_chat_boost() noexcept {
    auto* p = data.get_if<removed_chat_boost>();
    return p ? &p->value : nullptr;
  }
  const ChatBoostRemoved* get_removed_chat_boost() const noexcept {
    auto* p = data.get_if<removed_chat_boost>();
    return p ? &p->value : nullptr;
  }
  static constexpr decltype(auto) discriminate_field(std::string_view val, auto&& visitor) {
    if (val == "message")
      return visitor.template operator()<message>();
    if (val == "edited_message")
      return visitor.template operator()<edited_message>();
    if (val == "channel_post")
      return visitor.template operator()<channel_post>();
    if (val == "edited_channel_post")
      return visitor.template operator()<edited_channel_post>();
    if (val == "business_connection")
      return visitor.template operator()<business_connection>();
    if (val == "business_message")
      return visitor.template operator()<business_message>();
    if (val == "edited_business_message")
      return visitor.template operator()<edited_business_message>();
    if (val == "deleted_business_messages")
      return visitor.template operator()<deleted_business_messages>();
    if (val == "message_reaction")
      return visitor.template operator()<message_reaction>();
    if (val == "message_reaction_count")
      return visitor.template operator()<message_reaction_count>();
    if (val == "inline_query")
      return visitor.template operator()<inline_query>();
    if (val == "chosen_inline_result")
      return visitor.template operator()<chosen_inline_result>();
    if (val == "callback_query")
      return visitor.template operator()<callback_query>();
    if (val == "shipping_query")
      return visitor.template operator()<shipping_query>();
    if (val == "pre_checkout_query")
      return visitor.template operator()<pre_checkout_query>();
    if (val == "purchased_paid_media")
      return visitor.template operator()<purchased_paid_media>();
    if (val == "poll")
      return visitor.template operator()<poll>();
    if (val == "poll_answer")
      return visitor.template operator()<poll_answer>();
    if (val == "my_chat_member")
      return visitor.template operator()<my_chat_member>();
    if (val == "chat_member")
      return visitor.template operator()<chat_member>();
    if (val == "chat_join_request")
      return visitor.template operator()<chat_join_request>();
    if (val == "chat_boost")
      return visitor.template operator()<chat_boost>();
    if (val == "removed_chat_boost")
      return visitor.template operator()<removed_chat_boost>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum Update::type_e;
    switch (type()) {
      case k_message:
        return "message";
      case k_edited_message:
        return "edited_message";
      case k_channel_post:
        return "channel_post";
      case k_edited_channel_post:
        return "edited_channel_post";
      case k_business_connection:
        return "business_connection";
      case k_business_message:
        return "business_message";
      case k_edited_business_message:
        return "edited_business_message";
      case k_deleted_business_messages:
        return "deleted_business_messages";
      case k_message_reaction:
        return "message_reaction";
      case k_message_reaction_count:
        return "message_reaction_count";
      case k_inline_query:
        return "inline_query";
      case k_chosen_inline_result:
        return "chosen_inline_result";
      case k_callback_query:
        return "callback_query";
      case k_shipping_query:
        return "shipping_query";
      case k_pre_checkout_query:
        return "pre_checkout_query";
      case k_purchased_paid_media:
        return "purchased_paid_media";
      case k_poll:
        return "poll";
      case k_poll_answer:
        return "poll_answer";
      case k_my_chat_member:
        return "my_chat_member";
      case k_chat_member:
        return "chat_member";
      case k_chat_join_request:
        return "chat_join_request";
      case k_chat_boost:
        return "chat_boost";
      case k_removed_chat_boost:
        return "removed_chat_boost";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
