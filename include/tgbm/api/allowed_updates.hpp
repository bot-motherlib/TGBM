#pragma once

#include <string_view>

#include "tgbm/api/common.hpp"

namespace tgbm::api::allowed_updates {

constexpr const inline std::string_view values[]{
    "message",
    "edited_message",
    "channel_post",
    "edited_channel_post",
    "business_connection",
    "business_message",
    "edited_business_message",
    "deleted_business_messages",
    "message_reaction",        // not received by default
    "message_reaction_count",  // not received by default
    "inline_query",
    "chosen_inline_result",
    "callback_query",
    "shipping_query",
    "pre_checkout_query",
    "purchased_paid_media",
    "poll",
    "poll_answer",
    "my_chat_member",
    "chat_member",  // not received by default
    "chat_join_request",
    "chat_boost",
    "removed_chat_boost",
};

// has effect as specifying all except 'chat_member', 'message_reaction' and 'message_reaction_count'
inline arrayof<String> almost_all() noexcept {
  return arrayof<String>{};
}

inline arrayof<String> all() {
  arrayof<String> result;
  for (std::string_view s : values)
    result.emplace_back(s);
  return result;
}

[[nodiscard]] inline bool is_valid_update_category(std::string_view s) noexcept {
  return std::ranges::find(values, s) != std::end(values);
}

}  // namespace tgbm::api::allowed_updates
