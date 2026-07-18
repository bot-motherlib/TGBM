#pragma once

#include <tgbm/api/types/all_fwd.hpp>
#include <tgbm/api/types/InputTextMessageContent.hpp>
#include <tgbm/api/types/InputRichMessageContent.hpp>
#include <tgbm/api/types/InputLocationMessageContent.hpp>
#include <tgbm/api/types/InputVenueMessageContent.hpp>
#include <tgbm/api/types/InputContactMessageContent.hpp>
#include <tgbm/api/types/InputInvoiceMessageContent.hpp>

namespace tgbm::api {

/*This object represents the content of a message to be sent as a result of an inline query. Telegram clients
 * currently support the following types:*/
using InputMessageContent =
    oneof<InputTextMessageContent, InputRichMessageContent, InputLocationMessageContent,
          InputVenueMessageContent, InputContactMessageContent, InputInvoiceMessageContent>;

}  // namespace tgbm::api
