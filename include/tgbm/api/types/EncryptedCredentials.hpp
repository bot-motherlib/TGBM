#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes data required for decrypting and authenticating EncryptedPassportElement. See the Telegram
 * Passport Documentation for a complete description of the data decryption and authentication processes.*/
struct EncryptedCredentials {
  /* Base64-encoded encrypted JSON-serialized data with unique user's payload, data hashes and secrets
   * required for EncryptedPassportElement decryption and authentication */
  String data;
  /* Base64-encoded data hash for data authentication */
  String hash;
  /* Base64-encoded secret, encrypted with the bot's public RSA key, required for data decryption */
  String secret;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("data", true)
        .case_("hash", true)
        .case_("secret", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
