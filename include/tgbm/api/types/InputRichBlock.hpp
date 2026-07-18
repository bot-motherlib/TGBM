#pragma once

#include <tgbm/api/types/all_fwd.hpp>
#include <tgbm/api/types/InputRichBlockParagraph.hpp>
#include <tgbm/api/types/InputRichBlockSectionHeading.hpp>
#include <tgbm/api/types/InputRichBlockPreformatted.hpp>
#include <tgbm/api/types/InputRichBlockFooter.hpp>
#include <tgbm/api/types/InputRichBlockDivider.hpp>
#include <tgbm/api/types/InputRichBlockMathematicalExpression.hpp>
#include <tgbm/api/types/InputRichBlockAnchor.hpp>
#include <tgbm/api/types/InputRichBlockList.hpp>
#include <tgbm/api/types/InputRichBlockBlockQuotation.hpp>
#include <tgbm/api/types/InputRichBlockPullQuotation.hpp>
#include <tgbm/api/types/InputRichBlockCollage.hpp>
#include <tgbm/api/types/InputRichBlockSlideshow.hpp>
#include <tgbm/api/types/InputRichBlockTable.hpp>
#include <tgbm/api/types/InputRichBlockDetails.hpp>
#include <tgbm/api/types/InputRichBlockMap.hpp>
#include <tgbm/api/types/InputRichBlockAnimation.hpp>
#include <tgbm/api/types/InputRichBlockAudio.hpp>
#include <tgbm/api/types/InputRichBlockPhoto.hpp>
#include <tgbm/api/types/InputRichBlockVideo.hpp>
#include <tgbm/api/types/InputRichBlockVoiceNote.hpp>
#include <tgbm/api/types/InputRichBlockThinking.hpp>

namespace tgbm::api {

/*This object represents a block in a rich formatted message to be sent. Currently, it can be any of the
 * following types:*/
struct InputRichBlock {
  oneof<InputRichBlockParagraph, InputRichBlockSectionHeading, InputRichBlockPreformatted,
        InputRichBlockFooter, InputRichBlockDivider, InputRichBlockMathematicalExpression,
        InputRichBlockAnchor, InputRichBlockList, InputRichBlockBlockQuotation, InputRichBlockPullQuotation,
        InputRichBlockCollage, InputRichBlockSlideshow, InputRichBlockTable, InputRichBlockDetails,
        InputRichBlockMap, InputRichBlockAnimation, InputRichBlockAudio, InputRichBlockPhoto,
        InputRichBlockVideo, InputRichBlockVoiceNote, InputRichBlockThinking>
      data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_paragraph,
    k_sectionheading,
    k_preformatted,
    k_footer,
    k_divider,
    k_mathematicalexpression,
    k_anchor,
    k_list,
    k_blockquotation,
    k_pullquotation,
    k_collage,
    k_slideshow,
    k_table,
    k_details,
    k_map,
    k_animation,
    k_audio,
    k_photo,
    k_video,
    k_voicenote,
    k_thinking,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  InputRichBlockParagraph* get_paragraph() noexcept {
    return data.get_if<InputRichBlockParagraph>();
  }
  const InputRichBlockParagraph* get_paragraph() const noexcept {
    return data.get_if<InputRichBlockParagraph>();
  }
  InputRichBlockSectionHeading* get_sectionheading() noexcept {
    return data.get_if<InputRichBlockSectionHeading>();
  }
  const InputRichBlockSectionHeading* get_sectionheading() const noexcept {
    return data.get_if<InputRichBlockSectionHeading>();
  }
  InputRichBlockPreformatted* get_preformatted() noexcept {
    return data.get_if<InputRichBlockPreformatted>();
  }
  const InputRichBlockPreformatted* get_preformatted() const noexcept {
    return data.get_if<InputRichBlockPreformatted>();
  }
  InputRichBlockFooter* get_footer() noexcept {
    return data.get_if<InputRichBlockFooter>();
  }
  const InputRichBlockFooter* get_footer() const noexcept {
    return data.get_if<InputRichBlockFooter>();
  }
  InputRichBlockDivider* get_divider() noexcept {
    return data.get_if<InputRichBlockDivider>();
  }
  const InputRichBlockDivider* get_divider() const noexcept {
    return data.get_if<InputRichBlockDivider>();
  }
  InputRichBlockMathematicalExpression* get_mathematicalexpression() noexcept {
    return data.get_if<InputRichBlockMathematicalExpression>();
  }
  const InputRichBlockMathematicalExpression* get_mathematicalexpression() const noexcept {
    return data.get_if<InputRichBlockMathematicalExpression>();
  }
  InputRichBlockAnchor* get_anchor() noexcept {
    return data.get_if<InputRichBlockAnchor>();
  }
  const InputRichBlockAnchor* get_anchor() const noexcept {
    return data.get_if<InputRichBlockAnchor>();
  }
  InputRichBlockList* get_list() noexcept {
    return data.get_if<InputRichBlockList>();
  }
  const InputRichBlockList* get_list() const noexcept {
    return data.get_if<InputRichBlockList>();
  }
  InputRichBlockBlockQuotation* get_blockquotation() noexcept {
    return data.get_if<InputRichBlockBlockQuotation>();
  }
  const InputRichBlockBlockQuotation* get_blockquotation() const noexcept {
    return data.get_if<InputRichBlockBlockQuotation>();
  }
  InputRichBlockPullQuotation* get_pullquotation() noexcept {
    return data.get_if<InputRichBlockPullQuotation>();
  }
  const InputRichBlockPullQuotation* get_pullquotation() const noexcept {
    return data.get_if<InputRichBlockPullQuotation>();
  }
  InputRichBlockCollage* get_collage() noexcept {
    return data.get_if<InputRichBlockCollage>();
  }
  const InputRichBlockCollage* get_collage() const noexcept {
    return data.get_if<InputRichBlockCollage>();
  }
  InputRichBlockSlideshow* get_slideshow() noexcept {
    return data.get_if<InputRichBlockSlideshow>();
  }
  const InputRichBlockSlideshow* get_slideshow() const noexcept {
    return data.get_if<InputRichBlockSlideshow>();
  }
  InputRichBlockTable* get_table() noexcept {
    return data.get_if<InputRichBlockTable>();
  }
  const InputRichBlockTable* get_table() const noexcept {
    return data.get_if<InputRichBlockTable>();
  }
  InputRichBlockDetails* get_details() noexcept {
    return data.get_if<InputRichBlockDetails>();
  }
  const InputRichBlockDetails* get_details() const noexcept {
    return data.get_if<InputRichBlockDetails>();
  }
  InputRichBlockMap* get_map() noexcept {
    return data.get_if<InputRichBlockMap>();
  }
  const InputRichBlockMap* get_map() const noexcept {
    return data.get_if<InputRichBlockMap>();
  }
  InputRichBlockAnimation* get_animation() noexcept {
    return data.get_if<InputRichBlockAnimation>();
  }
  const InputRichBlockAnimation* get_animation() const noexcept {
    return data.get_if<InputRichBlockAnimation>();
  }
  InputRichBlockAudio* get_audio() noexcept {
    return data.get_if<InputRichBlockAudio>();
  }
  const InputRichBlockAudio* get_audio() const noexcept {
    return data.get_if<InputRichBlockAudio>();
  }
  InputRichBlockPhoto* get_photo() noexcept {
    return data.get_if<InputRichBlockPhoto>();
  }
  const InputRichBlockPhoto* get_photo() const noexcept {
    return data.get_if<InputRichBlockPhoto>();
  }
  InputRichBlockVideo* get_video() noexcept {
    return data.get_if<InputRichBlockVideo>();
  }
  const InputRichBlockVideo* get_video() const noexcept {
    return data.get_if<InputRichBlockVideo>();
  }
  InputRichBlockVoiceNote* get_voicenote() noexcept {
    return data.get_if<InputRichBlockVoiceNote>();
  }
  const InputRichBlockVoiceNote* get_voicenote() const noexcept {
    return data.get_if<InputRichBlockVoiceNote>();
  }
  InputRichBlockThinking* get_thinking() noexcept {
    return data.get_if<InputRichBlockThinking>();
  }
  const InputRichBlockThinking* get_thinking() const noexcept {
    return data.get_if<InputRichBlockThinking>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return string_switch<type_e>(val)
        .case_("paragraph", type_e::k_paragraph)
        .case_("heading", type_e::k_sectionheading)
        .case_("pre", type_e::k_preformatted)
        .case_("footer", type_e::k_footer)
        .case_("divider", type_e::k_divider)
        .case_("mathematical_expression", type_e::k_mathematicalexpression)
        .case_("anchor", type_e::k_anchor)
        .case_("list", type_e::k_list)
        .case_("blockquote", type_e::k_blockquotation)
        .case_("pullquote", type_e::k_pullquotation)
        .case_("collage", type_e::k_collage)
        .case_("slideshow", type_e::k_slideshow)
        .case_("table", type_e::k_table)
        .case_("details", type_e::k_details)
        .case_("map", type_e::k_map)
        .case_("animation", type_e::k_animation)
        .case_("audio", type_e::k_audio)
        .case_("photo", type_e::k_photo)
        .case_("video", type_e::k_video)
        .case_("voice_note", type_e::k_voicenote)
        .case_("thinking", type_e::k_thinking)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "paragraph")
      return visitor.template operator()<InputRichBlockParagraph>();
    if (val == "heading")
      return visitor.template operator()<InputRichBlockSectionHeading>();
    if (val == "pre")
      return visitor.template operator()<InputRichBlockPreformatted>();
    if (val == "footer")
      return visitor.template operator()<InputRichBlockFooter>();
    if (val == "divider")
      return visitor.template operator()<InputRichBlockDivider>();
    if (val == "mathematical_expression")
      return visitor.template operator()<InputRichBlockMathematicalExpression>();
    if (val == "anchor")
      return visitor.template operator()<InputRichBlockAnchor>();
    if (val == "list")
      return visitor.template operator()<InputRichBlockList>();
    if (val == "blockquote")
      return visitor.template operator()<InputRichBlockBlockQuotation>();
    if (val == "pullquote")
      return visitor.template operator()<InputRichBlockPullQuotation>();
    if (val == "collage")
      return visitor.template operator()<InputRichBlockCollage>();
    if (val == "slideshow")
      return visitor.template operator()<InputRichBlockSlideshow>();
    if (val == "table")
      return visitor.template operator()<InputRichBlockTable>();
    if (val == "details")
      return visitor.template operator()<InputRichBlockDetails>();
    if (val == "map")
      return visitor.template operator()<InputRichBlockMap>();
    if (val == "animation")
      return visitor.template operator()<InputRichBlockAnimation>();
    if (val == "audio")
      return visitor.template operator()<InputRichBlockAudio>();
    if (val == "photo")
      return visitor.template operator()<InputRichBlockPhoto>();
    if (val == "video")
      return visitor.template operator()<InputRichBlockVideo>();
    if (val == "voice_note")
      return visitor.template operator()<InputRichBlockVoiceNote>();
    if (val == "thinking")
      return visitor.template operator()<InputRichBlockThinking>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum InputRichBlock::type_e;
    switch (type()) {
      case k_paragraph:
        return "paragraph";
      case k_sectionheading:
        return "heading";
      case k_preformatted:
        return "pre";
      case k_footer:
        return "footer";
      case k_divider:
        return "divider";
      case k_mathematicalexpression:
        return "mathematical_expression";
      case k_anchor:
        return "anchor";
      case k_list:
        return "list";
      case k_blockquotation:
        return "blockquote";
      case k_pullquotation:
        return "pullquote";
      case k_collage:
        return "collage";
      case k_slideshow:
        return "slideshow";
      case k_table:
        return "table";
      case k_details:
        return "details";
      case k_map:
        return "map";
      case k_animation:
        return "animation";
      case k_audio:
        return "audio";
      case k_photo:
        return "photo";
      case k_video:
        return "video";
      case k_voicenote:
        return "voice_note";
      case k_thinking:
        return "thinking";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
