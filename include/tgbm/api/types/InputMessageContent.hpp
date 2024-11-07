#pragma once

#include "all_fwd.hpp"
#include "InputTextMessageContent.hpp"
#include "InputLocationMessageContent.hpp"
#include "InputVenueMessageContent.hpp"
#include "InputContactMessageContent.hpp"
#include "InputInvoiceMessageContent.hpp"

namespace tgbm::api {

/*This object represents the content of a message to be sent as a result of an inline query. Telegram clients
 * currently support the following 5 types:*/
using InputMessageContent =
    oneof<InputTextMessageContent, InputLocationMessageContent, InputVenueMessageContent,
          InputContactMessageContent, InputInvoiceMessageContent>;

}  // namespace tgbm::api
