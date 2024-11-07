#pragma once

#include "all_fwd.hpp"
#include "BotCommandScopeDefault.hpp"
#include "BotCommandScopeAllPrivateChats.hpp"
#include "BotCommandScopeAllGroupChats.hpp"
#include "BotCommandScopeAllChatAdministrators.hpp"
#include "BotCommandScopeChat.hpp"
#include "BotCommandScopeChatAdministrators.hpp"
#include "BotCommandScopeChatMember.hpp"

namespace tgbm::api {

/*This object represents the scope to which bot commands are applied. Currently, the following 7 scopes are
 * supported:*/
struct BotCommandScope {
  oneof<BotCommandScopeDefault, BotCommandScopeAllPrivateChats, BotCommandScopeAllGroupChats,
        BotCommandScopeAllChatAdministrators, BotCommandScopeChat, BotCommandScopeChatAdministrators,
        BotCommandScopeChatMember>
      data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_default,
    k_allprivatechats,
    k_allgroupchats,
    k_allchatadministrators,
    k_chat,
    k_chatadministrators,
    k_chatmember,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  BotCommandScopeDefault* get_default() noexcept {
    return data.get_if<BotCommandScopeDefault>();
  }
  const BotCommandScopeDefault* get_default() const noexcept {
    return data.get_if<BotCommandScopeDefault>();
  }
  BotCommandScopeAllPrivateChats* get_allprivatechats() noexcept {
    return data.get_if<BotCommandScopeAllPrivateChats>();
  }
  const BotCommandScopeAllPrivateChats* get_allprivatechats() const noexcept {
    return data.get_if<BotCommandScopeAllPrivateChats>();
  }
  BotCommandScopeAllGroupChats* get_allgroupchats() noexcept {
    return data.get_if<BotCommandScopeAllGroupChats>();
  }
  const BotCommandScopeAllGroupChats* get_allgroupchats() const noexcept {
    return data.get_if<BotCommandScopeAllGroupChats>();
  }
  BotCommandScopeAllChatAdministrators* get_allchatadministrators() noexcept {
    return data.get_if<BotCommandScopeAllChatAdministrators>();
  }
  const BotCommandScopeAllChatAdministrators* get_allchatadministrators() const noexcept {
    return data.get_if<BotCommandScopeAllChatAdministrators>();
  }
  BotCommandScopeChat* get_chat() noexcept {
    return data.get_if<BotCommandScopeChat>();
  }
  const BotCommandScopeChat* get_chat() const noexcept {
    return data.get_if<BotCommandScopeChat>();
  }
  BotCommandScopeChatAdministrators* get_chatadministrators() noexcept {
    return data.get_if<BotCommandScopeChatAdministrators>();
  }
  const BotCommandScopeChatAdministrators* get_chatadministrators() const noexcept {
    return data.get_if<BotCommandScopeChatAdministrators>();
  }
  BotCommandScopeChatMember* get_chatmember() noexcept {
    return data.get_if<BotCommandScopeChatMember>();
  }
  const BotCommandScopeChatMember* get_chatmember() const noexcept {
    return data.get_if<BotCommandScopeChatMember>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return utils::string_switch<type_e>(val)
        .case_("default", type_e::k_default)
        .case_("all_private_chats", type_e::k_allprivatechats)
        .case_("all_group_chats", type_e::k_allgroupchats)
        .case_("all_chat_administrators", type_e::k_allchatadministrators)
        .case_("chat", type_e::k_chat)
        .case_("chat_administrators", type_e::k_chatadministrators)
        .case_("chat_member", type_e::k_chatmember)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "default")
      return visitor.template operator()<BotCommandScopeDefault>();
    if (val == "all_private_chats")
      return visitor.template operator()<BotCommandScopeAllPrivateChats>();
    if (val == "all_group_chats")
      return visitor.template operator()<BotCommandScopeAllGroupChats>();
    if (val == "all_chat_administrators")
      return visitor.template operator()<BotCommandScopeAllChatAdministrators>();
    if (val == "chat")
      return visitor.template operator()<BotCommandScopeChat>();
    if (val == "chat_administrators")
      return visitor.template operator()<BotCommandScopeChatAdministrators>();
    if (val == "chat_member")
      return visitor.template operator()<BotCommandScopeChatMember>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum BotCommandScope::type_e;
    switch (type()) {
      case k_default:
        return "default";
      case k_allprivatechats:
        return "all_private_chats";
      case k_allgroupchats:
        return "all_group_chats";
      case k_allchatadministrators:
        return "all_chat_administrators";
      case k_chat:
        return "chat";
      case k_chatadministrators:
        return "chat_administrators";
      case k_chatmember:
        return "chat_member";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
