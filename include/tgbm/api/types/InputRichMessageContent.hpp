#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents the content of a message to be sent as a result of an inline query. Telegram clients
 * currently support the following types:*/
struct InputRichMessageContent {
  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
