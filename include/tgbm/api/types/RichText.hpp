#pragma once

#include <tgbm/api/types/all_fwd.hpp>
#include <tgbm/api/types/RichTextBold.hpp>
#include <tgbm/api/types/RichTextItalic.hpp>
#include <tgbm/api/types/RichTextUnderline.hpp>
#include <tgbm/api/types/RichTextStrikethrough.hpp>
#include <tgbm/api/types/RichTextSpoiler.hpp>
#include <tgbm/api/types/RichTextDateTime.hpp>
#include <tgbm/api/types/RichTextTextMention.hpp>
#include <tgbm/api/types/RichTextSubscript.hpp>
#include <tgbm/api/types/RichTextSuperscript.hpp>
#include <tgbm/api/types/RichTextMarked.hpp>
#include <tgbm/api/types/RichTextCode.hpp>
#include <tgbm/api/types/RichTextCustomEmoji.hpp>
#include <tgbm/api/types/RichTextMathematicalExpression.hpp>
#include <tgbm/api/types/RichTextUrl.hpp>
#include <tgbm/api/types/RichTextEmailAddress.hpp>
#include <tgbm/api/types/RichTextPhoneNumber.hpp>
#include <tgbm/api/types/RichTextBankCardNumber.hpp>
#include <tgbm/api/types/RichTextMention.hpp>
#include <tgbm/api/types/RichTextHashtag.hpp>
#include <tgbm/api/types/RichTextCashtag.hpp>
#include <tgbm/api/types/RichTextBotCommand.hpp>
#include <tgbm/api/types/RichTextAnchor.hpp>
#include <tgbm/api/types/RichTextAnchorLink.hpp>
#include <tgbm/api/types/RichTextReference.hpp>
#include <tgbm/api/types/RichTextReferenceLink.hpp>

namespace tgbm::api {

/*This object represents a rich formatted text. Currently, it can be either a String for plain text, an Array
 * of RichText, or any of the following types:*/
struct RichText {
  oneof<RichTextBold, RichTextItalic, RichTextUnderline, RichTextStrikethrough, RichTextSpoiler,
        RichTextDateTime, RichTextTextMention, RichTextSubscript, RichTextSuperscript, RichTextMarked,
        RichTextCode, RichTextCustomEmoji, RichTextMathematicalExpression, RichTextUrl, RichTextEmailAddress,
        RichTextPhoneNumber, RichTextBankCardNumber, RichTextMention, RichTextHashtag, RichTextCashtag,
        RichTextBotCommand, RichTextAnchor, RichTextAnchorLink, RichTextReference, RichTextReferenceLink>
      data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_bold,
    k_italic,
    k_underline,
    k_strikethrough,
    k_spoiler,
    k_datetime,
    k_textmention,
    k_subscript,
    k_superscript,
    k_marked,
    k_code,
    k_customemoji,
    k_mathematicalexpression,
    k_url,
    k_emailaddress,
    k_phonenumber,
    k_bankcardnumber,
    k_mention,
    k_hashtag,
    k_cashtag,
    k_botcommand,
    k_anchor,
    k_anchorlink,
    k_reference,
    k_referencelink,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  RichTextBold* get_bold() noexcept {
    return data.get_if<RichTextBold>();
  }
  const RichTextBold* get_bold() const noexcept {
    return data.get_if<RichTextBold>();
  }
  RichTextItalic* get_italic() noexcept {
    return data.get_if<RichTextItalic>();
  }
  const RichTextItalic* get_italic() const noexcept {
    return data.get_if<RichTextItalic>();
  }
  RichTextUnderline* get_underline() noexcept {
    return data.get_if<RichTextUnderline>();
  }
  const RichTextUnderline* get_underline() const noexcept {
    return data.get_if<RichTextUnderline>();
  }
  RichTextStrikethrough* get_strikethrough() noexcept {
    return data.get_if<RichTextStrikethrough>();
  }
  const RichTextStrikethrough* get_strikethrough() const noexcept {
    return data.get_if<RichTextStrikethrough>();
  }
  RichTextSpoiler* get_spoiler() noexcept {
    return data.get_if<RichTextSpoiler>();
  }
  const RichTextSpoiler* get_spoiler() const noexcept {
    return data.get_if<RichTextSpoiler>();
  }
  RichTextDateTime* get_datetime() noexcept {
    return data.get_if<RichTextDateTime>();
  }
  const RichTextDateTime* get_datetime() const noexcept {
    return data.get_if<RichTextDateTime>();
  }
  RichTextTextMention* get_textmention() noexcept {
    return data.get_if<RichTextTextMention>();
  }
  const RichTextTextMention* get_textmention() const noexcept {
    return data.get_if<RichTextTextMention>();
  }
  RichTextSubscript* get_subscript() noexcept {
    return data.get_if<RichTextSubscript>();
  }
  const RichTextSubscript* get_subscript() const noexcept {
    return data.get_if<RichTextSubscript>();
  }
  RichTextSuperscript* get_superscript() noexcept {
    return data.get_if<RichTextSuperscript>();
  }
  const RichTextSuperscript* get_superscript() const noexcept {
    return data.get_if<RichTextSuperscript>();
  }
  RichTextMarked* get_marked() noexcept {
    return data.get_if<RichTextMarked>();
  }
  const RichTextMarked* get_marked() const noexcept {
    return data.get_if<RichTextMarked>();
  }
  RichTextCode* get_code() noexcept {
    return data.get_if<RichTextCode>();
  }
  const RichTextCode* get_code() const noexcept {
    return data.get_if<RichTextCode>();
  }
  RichTextCustomEmoji* get_customemoji() noexcept {
    return data.get_if<RichTextCustomEmoji>();
  }
  const RichTextCustomEmoji* get_customemoji() const noexcept {
    return data.get_if<RichTextCustomEmoji>();
  }
  RichTextMathematicalExpression* get_mathematicalexpression() noexcept {
    return data.get_if<RichTextMathematicalExpression>();
  }
  const RichTextMathematicalExpression* get_mathematicalexpression() const noexcept {
    return data.get_if<RichTextMathematicalExpression>();
  }
  RichTextUrl* get_url() noexcept {
    return data.get_if<RichTextUrl>();
  }
  const RichTextUrl* get_url() const noexcept {
    return data.get_if<RichTextUrl>();
  }
  RichTextEmailAddress* get_emailaddress() noexcept {
    return data.get_if<RichTextEmailAddress>();
  }
  const RichTextEmailAddress* get_emailaddress() const noexcept {
    return data.get_if<RichTextEmailAddress>();
  }
  RichTextPhoneNumber* get_phonenumber() noexcept {
    return data.get_if<RichTextPhoneNumber>();
  }
  const RichTextPhoneNumber* get_phonenumber() const noexcept {
    return data.get_if<RichTextPhoneNumber>();
  }
  RichTextBankCardNumber* get_bankcardnumber() noexcept {
    return data.get_if<RichTextBankCardNumber>();
  }
  const RichTextBankCardNumber* get_bankcardnumber() const noexcept {
    return data.get_if<RichTextBankCardNumber>();
  }
  RichTextMention* get_mention() noexcept {
    return data.get_if<RichTextMention>();
  }
  const RichTextMention* get_mention() const noexcept {
    return data.get_if<RichTextMention>();
  }
  RichTextHashtag* get_hashtag() noexcept {
    return data.get_if<RichTextHashtag>();
  }
  const RichTextHashtag* get_hashtag() const noexcept {
    return data.get_if<RichTextHashtag>();
  }
  RichTextCashtag* get_cashtag() noexcept {
    return data.get_if<RichTextCashtag>();
  }
  const RichTextCashtag* get_cashtag() const noexcept {
    return data.get_if<RichTextCashtag>();
  }
  RichTextBotCommand* get_botcommand() noexcept {
    return data.get_if<RichTextBotCommand>();
  }
  const RichTextBotCommand* get_botcommand() const noexcept {
    return data.get_if<RichTextBotCommand>();
  }
  RichTextAnchor* get_anchor() noexcept {
    return data.get_if<RichTextAnchor>();
  }
  const RichTextAnchor* get_anchor() const noexcept {
    return data.get_if<RichTextAnchor>();
  }
  RichTextAnchorLink* get_anchorlink() noexcept {
    return data.get_if<RichTextAnchorLink>();
  }
  const RichTextAnchorLink* get_anchorlink() const noexcept {
    return data.get_if<RichTextAnchorLink>();
  }
  RichTextReference* get_reference() noexcept {
    return data.get_if<RichTextReference>();
  }
  const RichTextReference* get_reference() const noexcept {
    return data.get_if<RichTextReference>();
  }
  RichTextReferenceLink* get_referencelink() noexcept {
    return data.get_if<RichTextReferenceLink>();
  }
  const RichTextReferenceLink* get_referencelink() const noexcept {
    return data.get_if<RichTextReferenceLink>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return string_switch<type_e>(val)
        .case_("bold", type_e::k_bold)
        .case_("italic", type_e::k_italic)
        .case_("underline", type_e::k_underline)
        .case_("strikethrough", type_e::k_strikethrough)
        .case_("spoiler", type_e::k_spoiler)
        .case_("date_time", type_e::k_datetime)
        .case_("text_mention", type_e::k_textmention)
        .case_("subscript", type_e::k_subscript)
        .case_("superscript", type_e::k_superscript)
        .case_("marked", type_e::k_marked)
        .case_("code", type_e::k_code)
        .case_("custom_emoji", type_e::k_customemoji)
        .case_("mathematical_expression", type_e::k_mathematicalexpression)
        .case_("url", type_e::k_url)
        .case_("email_address", type_e::k_emailaddress)
        .case_("phone_number", type_e::k_phonenumber)
        .case_("bank_card_number", type_e::k_bankcardnumber)
        .case_("mention", type_e::k_mention)
        .case_("hashtag", type_e::k_hashtag)
        .case_("cashtag", type_e::k_cashtag)
        .case_("bot_command", type_e::k_botcommand)
        .case_("anchor", type_e::k_anchor)
        .case_("anchor_link", type_e::k_anchorlink)
        .case_("reference", type_e::k_reference)
        .case_("reference_link", type_e::k_referencelink)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "bold")
      return visitor.template operator()<RichTextBold>();
    if (val == "italic")
      return visitor.template operator()<RichTextItalic>();
    if (val == "underline")
      return visitor.template operator()<RichTextUnderline>();
    if (val == "strikethrough")
      return visitor.template operator()<RichTextStrikethrough>();
    if (val == "spoiler")
      return visitor.template operator()<RichTextSpoiler>();
    if (val == "date_time")
      return visitor.template operator()<RichTextDateTime>();
    if (val == "text_mention")
      return visitor.template operator()<RichTextTextMention>();
    if (val == "subscript")
      return visitor.template operator()<RichTextSubscript>();
    if (val == "superscript")
      return visitor.template operator()<RichTextSuperscript>();
    if (val == "marked")
      return visitor.template operator()<RichTextMarked>();
    if (val == "code")
      return visitor.template operator()<RichTextCode>();
    if (val == "custom_emoji")
      return visitor.template operator()<RichTextCustomEmoji>();
    if (val == "mathematical_expression")
      return visitor.template operator()<RichTextMathematicalExpression>();
    if (val == "url")
      return visitor.template operator()<RichTextUrl>();
    if (val == "email_address")
      return visitor.template operator()<RichTextEmailAddress>();
    if (val == "phone_number")
      return visitor.template operator()<RichTextPhoneNumber>();
    if (val == "bank_card_number")
      return visitor.template operator()<RichTextBankCardNumber>();
    if (val == "mention")
      return visitor.template operator()<RichTextMention>();
    if (val == "hashtag")
      return visitor.template operator()<RichTextHashtag>();
    if (val == "cashtag")
      return visitor.template operator()<RichTextCashtag>();
    if (val == "bot_command")
      return visitor.template operator()<RichTextBotCommand>();
    if (val == "anchor")
      return visitor.template operator()<RichTextAnchor>();
    if (val == "anchor_link")
      return visitor.template operator()<RichTextAnchorLink>();
    if (val == "reference")
      return visitor.template operator()<RichTextReference>();
    if (val == "reference_link")
      return visitor.template operator()<RichTextReferenceLink>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum RichText::type_e;
    switch (type()) {
      case k_bold:
        return "bold";
      case k_italic:
        return "italic";
      case k_underline:
        return "underline";
      case k_strikethrough:
        return "strikethrough";
      case k_spoiler:
        return "spoiler";
      case k_datetime:
        return "date_time";
      case k_textmention:
        return "text_mention";
      case k_subscript:
        return "subscript";
      case k_superscript:
        return "superscript";
      case k_marked:
        return "marked";
      case k_code:
        return "code";
      case k_customemoji:
        return "custom_emoji";
      case k_mathematicalexpression:
        return "mathematical_expression";
      case k_url:
        return "url";
      case k_emailaddress:
        return "email_address";
      case k_phonenumber:
        return "phone_number";
      case k_bankcardnumber:
        return "bank_card_number";
      case k_mention:
        return "mention";
      case k_hashtag:
        return "hashtag";
      case k_cashtag:
        return "cashtag";
      case k_botcommand:
        return "bot_command";
      case k_anchor:
        return "anchor";
      case k_anchorlink:
        return "anchor_link";
      case k_reference:
        return "reference";
      case k_referencelink:
        return "reference_link";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
