#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/InputStoryContent.hpp>
#include <tgbm/api/types/MessageEntity.hpp>
#include <tgbm/api/types/Story.hpp>
#include <tgbm/api/types/StoryArea.hpp>

namespace tgbm::api {

struct edit_story_request {
  /* Unique identifier of the business connection */
  String business_connection_id;
  /* Unique identifier of the story to edit */
  Integer story_id;
  /* Content of the story */
  box<InputStoryContent> content;
  /* Caption of the story, 0-2048 characters after entities parsing */
  optional<String> caption;
  /* Mode for parsing entities in the story caption. See formatting options for more details. */
  optional<String> parse_mode;
  /* A JSON-serialized list of special entities that appear in the caption, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> caption_entities;
  /* A JSON-serialized list of clickable areas to be shown on the story */
  optional<arrayof<StoryArea>> areas;

  using return_type = Story;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "editStory";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
    body.arg("story_id", story_id);
    body.arg("content", content);
    if (caption)
      body.arg("caption", *caption);
    if (parse_mode)
      body.arg("parse_mode", *parse_mode);
    if (caption_entities)
      body.arg("caption_entities", *caption_entities);
    if (areas)
      body.arg("areas", *areas);
  }
};

}  // namespace tgbm::api
