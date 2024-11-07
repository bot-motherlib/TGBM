#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*Describes documents or other Telegram Passport elements shared with the bot by the user.*/
struct EncryptedPassportElement {
  /* Element type. One of “personal_details”, “passport”, “driver_license”, “identity_card”,
   * “internal_passport”, “address”, “utility_bill”, “bank_statement”, “rental_agreement”,
   * “passport_registration”, “temporary_registration”, “phone_number”, “email”. */
  String type;
  /* Base64-encoded element hash for using in PassportElementErrorUnspecified */
  String hash;
  /* Optional. Base64-encoded encrypted Telegram Passport element data provided by the user; available only
   * for “personal_details”, “passport”, “driver_license”, “identity_card”, “internal_passport” and “address”
   * types. Can be decrypted and verified using the accompanying EncryptedCredentials. */
  optional<String> data;
  /* Optional. User's verified phone number; available only for “phone_number” type */
  optional<String> phone_number;
  /* Optional. User's verified email address; available only for “email” type */
  optional<String> email;
  /* Optional. Array of encrypted files with documents provided by the user; available only for
   * “utility_bill”, “bank_statement”, “rental_agreement”, “passport_registration” and
   * “temporary_registration” types. Files can be decrypted and verified using the accompanying
   * EncryptedCredentials. */
  optional<arrayof<PassportFile>> files;
  /* Optional. Encrypted file with the front side of the document, provided by the user; available only for
   * “passport”, “driver_license”, “identity_card” and “internal_passport”. The file can be decrypted and
   * verified using the accompanying EncryptedCredentials. */
  box<PassportFile> front_side;
  /* Optional. Encrypted file with the reverse side of the document, provided by the user; available only for
   * “driver_license” and “identity_card”. The file can be decrypted and verified using the accompanying
   * EncryptedCredentials. */
  box<PassportFile> reverse_side;
  /* Optional. Encrypted file with the selfie of the user holding a document, provided by the user; available
   * if requested for “passport”, “driver_license”, “identity_card” and “internal_passport”. The file can be
   * decrypted and verified using the accompanying EncryptedCredentials. */
  box<PassportFile> selfie;
  /* Optional. Array of encrypted files with translated versions of documents provided by the user; available
   * if requested for “passport”, “driver_license”, “identity_card”, “internal_passport”, “utility_bill”,
   * “bank_statement”, “rental_agreement”, “passport_registration” and “temporary_registration” types. Files
   * can be decrypted and verified using the accompanying EncryptedCredentials. */
  optional<arrayof<PassportFile>> translation;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).case_("type", true).case_("hash", true).or_default(false);
  }
};

}  // namespace tgbm::api
