#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a service message about the approval of a suggested post.*/
struct SuggestedPostApproved {
  /* Date when the post will be published */
  Integer send_date;
  /* Optional. Message containing the suggested post. Note that the Message object in this field will not
   * contain the reply_to_message field even if it itself is a reply. */
  box<Message> suggested_post_message;
  /* Optional. Amount paid for the post */
  box<SuggestedPostPrice> price;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("send_date", true).or_default(false);
  }
};

}  // namespace tgbm::api
