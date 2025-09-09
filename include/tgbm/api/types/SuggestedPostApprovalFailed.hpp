#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a service message about the failed approval of a suggested post. Currently, only caused by
 * insufficient user funds at the time of approval.*/
struct SuggestedPostApprovalFailed {
  /* Expected price of the post */
  box<SuggestedPostPrice> price;
  /* Optional. Message containing the suggested post whose approval has failed. Note that the Message object
   * in this field will not contain the reply_to_message field even if it itself is a reply. */
  box<Message> suggested_post_message;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("price", true).or_default(false);
  }
};

}  // namespace tgbm::api
