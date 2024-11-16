#pragma once

#include <tgbm/api/types/all_fwd.hpp>
#include <tgbm/api/types/MessageOriginUser.hpp>
#include <tgbm/api/types/MessageOriginHiddenUser.hpp>
#include <tgbm/api/types/MessageOriginChat.hpp>
#include <tgbm/api/types/MessageOriginChannel.hpp>

namespace tgbm::api {

/*This object describes the origin of a message. It can be one of*/
struct MessageOrigin {
  oneof<MessageOriginUser, MessageOriginHiddenUser, MessageOriginChat, MessageOriginChannel> data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_user,
    k_hiddenuser,
    k_chat,
    k_channel,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  MessageOriginUser* get_user() noexcept {
    return data.get_if<MessageOriginUser>();
  }
  const MessageOriginUser* get_user() const noexcept {
    return data.get_if<MessageOriginUser>();
  }
  MessageOriginHiddenUser* get_hiddenuser() noexcept {
    return data.get_if<MessageOriginHiddenUser>();
  }
  const MessageOriginHiddenUser* get_hiddenuser() const noexcept {
    return data.get_if<MessageOriginHiddenUser>();
  }
  MessageOriginChat* get_chat() noexcept {
    return data.get_if<MessageOriginChat>();
  }
  const MessageOriginChat* get_chat() const noexcept {
    return data.get_if<MessageOriginChat>();
  }
  MessageOriginChannel* get_channel() noexcept {
    return data.get_if<MessageOriginChannel>();
  }
  const MessageOriginChannel* get_channel() const noexcept {
    return data.get_if<MessageOriginChannel>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return string_switch<type_e>(val)
        .case_("user", type_e::k_user)
        .case_("hidden_user", type_e::k_hiddenuser)
        .case_("chat", type_e::k_chat)
        .case_("channel", type_e::k_channel)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "user")
      return visitor.template operator()<MessageOriginUser>();
    if (val == "hidden_user")
      return visitor.template operator()<MessageOriginHiddenUser>();
    if (val == "chat")
      return visitor.template operator()<MessageOriginChat>();
    if (val == "channel")
      return visitor.template operator()<MessageOriginChannel>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum MessageOrigin::type_e;
    switch (type()) {
      case k_user:
        return "user";
      case k_hiddenuser:
        return "hidden_user";
      case k_chat:
        return "chat";
      case k_channel:
        return "channel";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
