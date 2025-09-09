#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a service message about the rejection of a suggested post.*/
struct SuggestedPostDeclined {
  /* Optional. Message containing the suggested post. Note that the Message object in this field will not
   * contain the reply_to_message field even if it itself is a reply. */
  box<Message> suggested_post_message;
  /* Optional. Comment with which the post was declined */
  optional<String> comment;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
