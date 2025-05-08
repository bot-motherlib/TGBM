#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/OwnedGifts.hpp>

namespace tgbm::api {

struct get_business_account_gifts_request {
  /* Unique identifier of the business connection */
  String business_connection_id;
  /* Pass True to exclude gifts that aren't saved to the account's profile page */
  optional<bool> exclude_unsaved;
  /* Pass True to exclude gifts that are saved to the account's profile page */
  optional<bool> exclude_saved;
  /* Pass True to exclude gifts that can be purchased an unlimited number of times */
  optional<bool> exclude_unlimited;
  /* Pass True to exclude gifts that can be purchased a limited number of times */
  optional<bool> exclude_limited;
  /* Pass True to exclude unique gifts */
  optional<bool> exclude_unique;
  /* Pass True to sort results by gift price instead of send date. Sorting is applied before pagination. */
  optional<bool> sort_by_price;
  /* Offset of the first entry to return as received from the previous request; use empty string to get the
   * first chunk of results */
  optional<String> offset;
  /* The maximum number of gifts to be returned; 1-100. Defaults to 100 */
  optional<Integer> limit;

  using return_type = OwnedGifts;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getBusinessAccountGifts";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
    if (exclude_unsaved)
      body.arg("exclude_unsaved", *exclude_unsaved);
    if (exclude_saved)
      body.arg("exclude_saved", *exclude_saved);
    if (exclude_unlimited)
      body.arg("exclude_unlimited", *exclude_unlimited);
    if (exclude_limited)
      body.arg("exclude_limited", *exclude_limited);
    if (exclude_unique)
      body.arg("exclude_unique", *exclude_unique);
    if (sort_by_price)
      body.arg("sort_by_price", *sort_by_price);
    if (offset)
      body.arg("offset", *offset);
    if (limit)
      body.arg("limit", *limit);
  }
};

}  // namespace tgbm::api
