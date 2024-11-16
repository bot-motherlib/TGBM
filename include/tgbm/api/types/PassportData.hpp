#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes Telegram Passport data shared with the bot by the user.*/
struct PassportData {
  /* Array with information about documents and other Telegram Passport elements that was shared with the bot
   */
  arrayof<EncryptedPassportElement> data;
  /* Encrypted credentials required to decrypt the data */
  box<EncryptedCredentials> credentials;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("data", true).case_("credentials", true).or_default(false);
  }
};

}  // namespace tgbm::api
