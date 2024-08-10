#include "tgbm/types/InputTextMessageContent.h"
#include "tgbm/types/InputLocationMessageContent.h"
#include "tgbm/types/InputVenueMessageContent.h"
#include "tgbm/types/InputContactMessageContent.h"
#include "tgbm/types/InputInvoiceMessageContent.h"

#include <string>

using namespace tgbm;

const std::string InputTextMessageContent::TYPE = "text";
const std::string InputLocationMessageContent::TYPE = "location";
const std::string InputVenueMessageContent::TYPE = "venue";
const std::string InputContactMessageContent::TYPE = "contact";
const std::string InputInvoiceMessageContent::TYPE = "invoice";
