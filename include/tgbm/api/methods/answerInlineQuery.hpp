#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/InlineQueryResult.hpp"
#include "tgbm/api/types/InlineQueryResultsButton.hpp"

namespace tgbm::api {

struct answer_inline_query_request {
  /* Unique identifier for the answered query */
  String inline_query_id;
  /* A JSON-serialized array of results for the inline query */
  arrayof<InlineQueryResult> results;
  /* The maximum amount of time in seconds that the result of the inline query may be cached on the server.
   * Defaults to 300. */
  optional<Integer> cache_time;
  /* Pass True if results may be cached on the server side only for the user that sent the query. By default,
   * results may be returned to any user who sends the same query. */
  optional<bool> is_personal;
  /* Pass the offset that a client should send in the next query with the same text to receive more results.
   * Pass an empty string if there are no more results or if you don't support pagination. Offset length can't
   * exceed 64 bytes. */
  optional<String> next_offset;
  /* A JSON-serialized object describing a button to be shown above inline query results */
  box<InlineQueryResultsButton> button;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "answerInlineQuery";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("inline_query_id", inline_query_id);
    body.arg("results", results);
    if (cache_time)
      body.arg("cache_time", *cache_time);
    if (is_personal)
      body.arg("is_personal", *is_personal);
    if (next_offset)
      body.arg("next_offset", *next_offset);
    if (button)
      body.arg("button", *button);
  }
};

}  // namespace tgbm::api
