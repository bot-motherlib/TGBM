#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/OwnedGifts.hpp>

namespace tgbm::api {

struct get_user_gifts_request {
  /* Unique identifier of the user */
  Integer user_id;
  /* Pass True to exclude gifts that can be purchased an unlimited number of times */
  optional<bool> exclude_unlimited;
  /* Pass True to exclude gifts that can be purchased a limited number of times and can be upgraded to unique
   */
  optional<bool> exclude_limited_upgradable;
  /* Pass True to exclude gifts that can be purchased a limited number of times and can't be upgraded to
   * unique */
  optional<bool> exclude_limited_non_upgradable;
  /* Pass True to exclude gifts that were assigned from the TON blockchain and can't be resold or transferred
   * in Telegram */
  optional<bool> exclude_from_blockchain;
  /* Pass True to exclude unique gifts */
  optional<bool> exclude_unique;
  /* Pass True to sort results by gift price instead of send date. Sorting is applied before pagination. */
  optional<bool> sort_by_price;
  /* Offset of the first entry to return as received from the previous request; use an empty string to get the
   * first chunk of results */
  optional<String> offset;
  /* The maximum number of gifts to be returned; 1-100. Defaults to 100 */
  optional<Integer> limit;

  using return_type = OwnedGifts;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getUserGifts";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
    if (exclude_unlimited)
      body.arg("exclude_unlimited", *exclude_unlimited);
    if (exclude_limited_upgradable)
      body.arg("exclude_limited_upgradable", *exclude_limited_upgradable);
    if (exclude_limited_non_upgradable)
      body.arg("exclude_limited_non_upgradable", *exclude_limited_non_upgradable);
    if (exclude_from_blockchain)
      body.arg("exclude_from_blockchain", *exclude_from_blockchain);
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
