#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/Story.hpp>

namespace tgbm::api {

struct repost_story_request {
  /* Unique identifier of the business connection */
  String business_connection_id;
  /* Unique identifier of the chat which posted the story that should be reposted */
  Integer from_chat_id;
  /* Unique identifier of the story that should be reposted */
  Integer from_story_id;
  /* Period after which the story is moved to the archive, in seconds; must be one of 6 * 3600, 12 * 3600,
   * 86400, or 2 * 86400 */
  Integer active_period;
  /* Pass True to keep the story accessible after it expires */
  optional<bool> post_to_chat_page;
  /* Pass True if the content of the story must be protected from forwarding and screenshotting */
  optional<bool> protect_content;

  using return_type = Story;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "repostStory";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
    body.arg("from_chat_id", from_chat_id);
    body.arg("from_story_id", from_story_id);
    body.arg("active_period", active_period);
    if (post_to_chat_page)
      body.arg("post_to_chat_page", *post_to_chat_page);
    if (protect_content)
      body.arg("protect_content", *protect_content);
  }
};

}  // namespace tgbm::api
