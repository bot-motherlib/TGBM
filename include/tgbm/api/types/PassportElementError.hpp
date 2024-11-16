#pragma once

#include <tgbm/api/types/all_fwd.hpp>
#include <tgbm/api/types/PassportElementErrorDataField.hpp>
#include <tgbm/api/types/PassportElementErrorFrontSide.hpp>
#include <tgbm/api/types/PassportElementErrorReverseSide.hpp>
#include <tgbm/api/types/PassportElementErrorSelfie.hpp>
#include <tgbm/api/types/PassportElementErrorFile.hpp>
#include <tgbm/api/types/PassportElementErrorFiles.hpp>
#include <tgbm/api/types/PassportElementErrorTranslationFile.hpp>
#include <tgbm/api/types/PassportElementErrorTranslationFiles.hpp>
#include <tgbm/api/types/PassportElementErrorUnspecified.hpp>

namespace tgbm::api {

/*This object represents an error in the Telegram Passport element which was submitted that should be resolved
 * by the user. It should be one of:*/
struct PassportElementError {
  oneof<PassportElementErrorDataField, PassportElementErrorFrontSide, PassportElementErrorReverseSide,
        PassportElementErrorSelfie, PassportElementErrorFile, PassportElementErrorFiles,
        PassportElementErrorTranslationFile, PassportElementErrorTranslationFiles,
        PassportElementErrorUnspecified>
      data;
  static constexpr std::string_view discriminator = "source";
  enum struct type_e {
    k_datafield,
    k_frontside,
    k_reverseside,
    k_selfie,
    k_file,
    k_files,
    k_translationfile,
    k_translationfiles,
    k_unspecified,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  PassportElementErrorDataField* get_datafield() noexcept {
    return data.get_if<PassportElementErrorDataField>();
  }
  const PassportElementErrorDataField* get_datafield() const noexcept {
    return data.get_if<PassportElementErrorDataField>();
  }
  PassportElementErrorFrontSide* get_frontside() noexcept {
    return data.get_if<PassportElementErrorFrontSide>();
  }
  const PassportElementErrorFrontSide* get_frontside() const noexcept {
    return data.get_if<PassportElementErrorFrontSide>();
  }
  PassportElementErrorReverseSide* get_reverseside() noexcept {
    return data.get_if<PassportElementErrorReverseSide>();
  }
  const PassportElementErrorReverseSide* get_reverseside() const noexcept {
    return data.get_if<PassportElementErrorReverseSide>();
  }
  PassportElementErrorSelfie* get_selfie() noexcept {
    return data.get_if<PassportElementErrorSelfie>();
  }
  const PassportElementErrorSelfie* get_selfie() const noexcept {
    return data.get_if<PassportElementErrorSelfie>();
  }
  PassportElementErrorFile* get_file() noexcept {
    return data.get_if<PassportElementErrorFile>();
  }
  const PassportElementErrorFile* get_file() const noexcept {
    return data.get_if<PassportElementErrorFile>();
  }
  PassportElementErrorFiles* get_files() noexcept {
    return data.get_if<PassportElementErrorFiles>();
  }
  const PassportElementErrorFiles* get_files() const noexcept {
    return data.get_if<PassportElementErrorFiles>();
  }
  PassportElementErrorTranslationFile* get_translationfile() noexcept {
    return data.get_if<PassportElementErrorTranslationFile>();
  }
  const PassportElementErrorTranslationFile* get_translationfile() const noexcept {
    return data.get_if<PassportElementErrorTranslationFile>();
  }
  PassportElementErrorTranslationFiles* get_translationfiles() noexcept {
    return data.get_if<PassportElementErrorTranslationFiles>();
  }
  const PassportElementErrorTranslationFiles* get_translationfiles() const noexcept {
    return data.get_if<PassportElementErrorTranslationFiles>();
  }
  PassportElementErrorUnspecified* get_unspecified() noexcept {
    return data.get_if<PassportElementErrorUnspecified>();
  }
  const PassportElementErrorUnspecified* get_unspecified() const noexcept {
    return data.get_if<PassportElementErrorUnspecified>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return string_switch<type_e>(val)
        .case_("data", type_e::k_datafield)
        .case_("front_side", type_e::k_frontside)
        .case_("reverse_side", type_e::k_reverseside)
        .case_("selfie", type_e::k_selfie)
        .case_("file", type_e::k_file)
        .case_("files", type_e::k_files)
        .case_("translation_file", type_e::k_translationfile)
        .case_("translation_files", type_e::k_translationfiles)
        .case_("unspecified", type_e::k_unspecified)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "data")
      return visitor.template operator()<PassportElementErrorDataField>();
    if (val == "front_side")
      return visitor.template operator()<PassportElementErrorFrontSide>();
    if (val == "reverse_side")
      return visitor.template operator()<PassportElementErrorReverseSide>();
    if (val == "selfie")
      return visitor.template operator()<PassportElementErrorSelfie>();
    if (val == "file")
      return visitor.template operator()<PassportElementErrorFile>();
    if (val == "files")
      return visitor.template operator()<PassportElementErrorFiles>();
    if (val == "translation_file")
      return visitor.template operator()<PassportElementErrorTranslationFile>();
    if (val == "translation_files")
      return visitor.template operator()<PassportElementErrorTranslationFiles>();
    if (val == "unspecified")
      return visitor.template operator()<PassportElementErrorUnspecified>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum PassportElementError::type_e;
    switch (type()) {
      case k_datafield:
        return "data";
      case k_frontside:
        return "front_side";
      case k_reverseside:
        return "reverse_side";
      case k_selfie:
        return "selfie";
      case k_file:
        return "file";
      case k_files:
        return "files";
      case k_translationfile:
        return "translation_file";
      case k_translationfiles:
        return "translation_files";
      case k_unspecified:
        return "unspecified";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
