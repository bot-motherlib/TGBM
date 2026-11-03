#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A block with a general file, corresponding to the custom HTML tag <tg-document>.*/
struct InputRichBlockDocument {
  /* The document. Caption is ignored. */
  box<InputMediaDocument> document;
  /* Optional. Caption of the block */
  box<RichBlockCaption> caption;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("document", true).or_default(false);
  }
};

}  // namespace tgbm::api
