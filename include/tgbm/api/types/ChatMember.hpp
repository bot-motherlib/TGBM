#pragma once

#include <tgbm/api/types/all_fwd.hpp>
#include <tgbm/api/types/ChatMemberOwner.hpp>
#include <tgbm/api/types/ChatMemberAdministrator.hpp>
#include <tgbm/api/types/ChatMemberMember.hpp>
#include <tgbm/api/types/ChatMemberRestricted.hpp>
#include <tgbm/api/types/ChatMemberLeft.hpp>
#include <tgbm/api/types/ChatMemberBanned.hpp>

namespace tgbm::api {

/*This object contains information about one member of a chat. Currently, the following 6 types of chat
 * members are supported:*/
struct ChatMember {
  oneof<ChatMemberOwner, ChatMemberAdministrator, ChatMemberMember, ChatMemberRestricted, ChatMemberLeft,
        ChatMemberBanned>
      data;
  static constexpr std::string_view discriminator = "status";
  enum struct type_e {
    k_owner,
    k_administrator,
    k_member,
    k_restricted,
    k_left,
    k_banned,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  ChatMemberOwner* get_owner() noexcept {
    return data.get_if<ChatMemberOwner>();
  }
  const ChatMemberOwner* get_owner() const noexcept {
    return data.get_if<ChatMemberOwner>();
  }
  ChatMemberAdministrator* get_administrator() noexcept {
    return data.get_if<ChatMemberAdministrator>();
  }
  const ChatMemberAdministrator* get_administrator() const noexcept {
    return data.get_if<ChatMemberAdministrator>();
  }
  ChatMemberMember* get_member() noexcept {
    return data.get_if<ChatMemberMember>();
  }
  const ChatMemberMember* get_member() const noexcept {
    return data.get_if<ChatMemberMember>();
  }
  ChatMemberRestricted* get_restricted() noexcept {
    return data.get_if<ChatMemberRestricted>();
  }
  const ChatMemberRestricted* get_restricted() const noexcept {
    return data.get_if<ChatMemberRestricted>();
  }
  ChatMemberLeft* get_left() noexcept {
    return data.get_if<ChatMemberLeft>();
  }
  const ChatMemberLeft* get_left() const noexcept {
    return data.get_if<ChatMemberLeft>();
  }
  ChatMemberBanned* get_banned() noexcept {
    return data.get_if<ChatMemberBanned>();
  }
  const ChatMemberBanned* get_banned() const noexcept {
    return data.get_if<ChatMemberBanned>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return string_switch<type_e>(val)
        .case_("creator", type_e::k_owner)
        .case_("administrator", type_e::k_administrator)
        .case_("member", type_e::k_member)
        .case_("restricted", type_e::k_restricted)
        .case_("left", type_e::k_left)
        .case_("kicked", type_e::k_banned)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "creator")
      return visitor.template operator()<ChatMemberOwner>();
    if (val == "administrator")
      return visitor.template operator()<ChatMemberAdministrator>();
    if (val == "member")
      return visitor.template operator()<ChatMemberMember>();
    if (val == "restricted")
      return visitor.template operator()<ChatMemberRestricted>();
    if (val == "left")
      return visitor.template operator()<ChatMemberLeft>();
    if (val == "kicked")
      return visitor.template operator()<ChatMemberBanned>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum ChatMember::type_e;
    switch (type()) {
      case k_owner:
        return "creator";
      case k_administrator:
        return "administrator";
      case k_member:
        return "member";
      case k_restricted:
        return "restricted";
      case k_left:
        return "left";
      case k_banned:
        return "kicked";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
