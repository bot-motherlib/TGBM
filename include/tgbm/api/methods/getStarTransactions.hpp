#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/StarTransactions.hpp>

namespace tgbm::api {

struct get_star_transactions_request {
  /* Number of transactions to skip in the response */
  optional<Integer> offset;
  /* The maximum number of transactions to be retrieved. Values between 1-100 are accepted. Defaults to 100.
   */
  optional<Integer> limit;

  using return_type = StarTransactions;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getStarTransactions";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (offset)
      body.arg("offset", *offset);
    if (limit)
      body.arg("limit", *limit);
  }
};

}  // namespace tgbm::api
