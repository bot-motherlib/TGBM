#pragma once

#include <tgbm/api/types/all_fwd.hpp>
#include <tgbm/api/types/RichBlockParagraph.hpp>
#include <tgbm/api/types/RichBlockSectionHeading.hpp>
#include <tgbm/api/types/RichBlockPreformatted.hpp>
#include <tgbm/api/types/RichBlockFooter.hpp>
#include <tgbm/api/types/RichBlockDivider.hpp>
#include <tgbm/api/types/RichBlockMathematicalExpression.hpp>
#include <tgbm/api/types/RichBlockAnchor.hpp>
#include <tgbm/api/types/RichBlockList.hpp>
#include <tgbm/api/types/RichBlockBlockQuotation.hpp>
#include <tgbm/api/types/RichBlockPullQuotation.hpp>
#include <tgbm/api/types/RichBlockCollage.hpp>
#include <tgbm/api/types/RichBlockSlideshow.hpp>
#include <tgbm/api/types/RichBlockTable.hpp>
#include <tgbm/api/types/RichBlockDetails.hpp>
#include <tgbm/api/types/RichBlockMap.hpp>
#include <tgbm/api/types/RichBlockAnimation.hpp>
#include <tgbm/api/types/RichBlockAudio.hpp>
#include <tgbm/api/types/RichBlockPhoto.hpp>
#include <tgbm/api/types/RichBlockVideo.hpp>
#include <tgbm/api/types/RichBlockVoiceNote.hpp>
#include <tgbm/api/types/RichBlockThinking.hpp>

namespace tgbm::api {

/*This object represents a block in a rich formatted message. Currently, it can be any of the following
 * types:*/
struct RichBlock {
  oneof<RichBlockParagraph, RichBlockSectionHeading, RichBlockPreformatted, RichBlockFooter, RichBlockDivider,
        RichBlockMathematicalExpression, RichBlockAnchor, RichBlockList, RichBlockBlockQuotation,
        RichBlockPullQuotation, RichBlockCollage, RichBlockSlideshow, RichBlockTable, RichBlockDetails,
        RichBlockMap, RichBlockAnimation, RichBlockAudio, RichBlockPhoto, RichBlockVideo, RichBlockVoiceNote,
        RichBlockThinking>
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
  RichBlockParagraph* get_paragraph() noexcept {
    return data.get_if<RichBlockParagraph>();
  }
  const RichBlockParagraph* get_paragraph() const noexcept {
    return data.get_if<RichBlockParagraph>();
  }
  RichBlockSectionHeading* get_sectionheading() noexcept {
    return data.get_if<RichBlockSectionHeading>();
  }
  const RichBlockSectionHeading* get_sectionheading() const noexcept {
    return data.get_if<RichBlockSectionHeading>();
  }
  RichBlockPreformatted* get_preformatted() noexcept {
    return data.get_if<RichBlockPreformatted>();
  }
  const RichBlockPreformatted* get_preformatted() const noexcept {
    return data.get_if<RichBlockPreformatted>();
  }
  RichBlockFooter* get_footer() noexcept {
    return data.get_if<RichBlockFooter>();
  }
  const RichBlockFooter* get_footer() const noexcept {
    return data.get_if<RichBlockFooter>();
  }
  RichBlockDivider* get_divider() noexcept {
    return data.get_if<RichBlockDivider>();
  }
  const RichBlockDivider* get_divider() const noexcept {
    return data.get_if<RichBlockDivider>();
  }
  RichBlockMathematicalExpression* get_mathematicalexpression() noexcept {
    return data.get_if<RichBlockMathematicalExpression>();
  }
  const RichBlockMathematicalExpression* get_mathematicalexpression() const noexcept {
    return data.get_if<RichBlockMathematicalExpression>();
  }
  RichBlockAnchor* get_anchor() noexcept {
    return data.get_if<RichBlockAnchor>();
  }
  const RichBlockAnchor* get_anchor() const noexcept {
    return data.get_if<RichBlockAnchor>();
  }
  RichBlockList* get_list() noexcept {
    return data.get_if<RichBlockList>();
  }
  const RichBlockList* get_list() const noexcept {
    return data.get_if<RichBlockList>();
  }
  RichBlockBlockQuotation* get_blockquotation() noexcept {
    return data.get_if<RichBlockBlockQuotation>();
  }
  const RichBlockBlockQuotation* get_blockquotation() const noexcept {
    return data.get_if<RichBlockBlockQuotation>();
  }
  RichBlockPullQuotation* get_pullquotation() noexcept {
    return data.get_if<RichBlockPullQuotation>();
  }
  const RichBlockPullQuotation* get_pullquotation() const noexcept {
    return data.get_if<RichBlockPullQuotation>();
  }
  RichBlockCollage* get_collage() noexcept {
    return data.get_if<RichBlockCollage>();
  }
  const RichBlockCollage* get_collage() const noexcept {
    return data.get_if<RichBlockCollage>();
  }
  RichBlockSlideshow* get_slideshow() noexcept {
    return data.get_if<RichBlockSlideshow>();
  }
  const RichBlockSlideshow* get_slideshow() const noexcept {
    return data.get_if<RichBlockSlideshow>();
  }
  RichBlockTable* get_table() noexcept {
    return data.get_if<RichBlockTable>();
  }
  const RichBlockTable* get_table() const noexcept {
    return data.get_if<RichBlockTable>();
  }
  RichBlockDetails* get_details() noexcept {
    return data.get_if<RichBlockDetails>();
  }
  const RichBlockDetails* get_details() const noexcept {
    return data.get_if<RichBlockDetails>();
  }
  RichBlockMap* get_map() noexcept {
    return data.get_if<RichBlockMap>();
  }
  const RichBlockMap* get_map() const noexcept {
    return data.get_if<RichBlockMap>();
  }
  RichBlockAnimation* get_animation() noexcept {
    return data.get_if<RichBlockAnimation>();
  }
  const RichBlockAnimation* get_animation() const noexcept {
    return data.get_if<RichBlockAnimation>();
  }
  RichBlockAudio* get_audio() noexcept {
    return data.get_if<RichBlockAudio>();
  }
  const RichBlockAudio* get_audio() const noexcept {
    return data.get_if<RichBlockAudio>();
  }
  RichBlockPhoto* get_photo() noexcept {
    return data.get_if<RichBlockPhoto>();
  }
  const RichBlockPhoto* get_photo() const noexcept {
    return data.get_if<RichBlockPhoto>();
  }
  RichBlockVideo* get_video() noexcept {
    return data.get_if<RichBlockVideo>();
  }
  const RichBlockVideo* get_video() const noexcept {
    return data.get_if<RichBlockVideo>();
  }
  RichBlockVoiceNote* get_voicenote() noexcept {
    return data.get_if<RichBlockVoiceNote>();
  }
  const RichBlockVoiceNote* get_voicenote() const noexcept {
    return data.get_if<RichBlockVoiceNote>();
  }
  RichBlockThinking* get_thinking() noexcept {
    return data.get_if<RichBlockThinking>();
  }
  const RichBlockThinking* get_thinking() const noexcept {
    return data.get_if<RichBlockThinking>();
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
      return visitor.template operator()<RichBlockParagraph>();
    if (val == "heading")
      return visitor.template operator()<RichBlockSectionHeading>();
    if (val == "pre")
      return visitor.template operator()<RichBlockPreformatted>();
    if (val == "footer")
      return visitor.template operator()<RichBlockFooter>();
    if (val == "divider")
      return visitor.template operator()<RichBlockDivider>();
    if (val == "mathematical_expression")
      return visitor.template operator()<RichBlockMathematicalExpression>();
    if (val == "anchor")
      return visitor.template operator()<RichBlockAnchor>();
    if (val == "list")
      return visitor.template operator()<RichBlockList>();
    if (val == "blockquote")
      return visitor.template operator()<RichBlockBlockQuotation>();
    if (val == "pullquote")
      return visitor.template operator()<RichBlockPullQuotation>();
    if (val == "collage")
      return visitor.template operator()<RichBlockCollage>();
    if (val == "slideshow")
      return visitor.template operator()<RichBlockSlideshow>();
    if (val == "table")
      return visitor.template operator()<RichBlockTable>();
    if (val == "details")
      return visitor.template operator()<RichBlockDetails>();
    if (val == "map")
      return visitor.template operator()<RichBlockMap>();
    if (val == "animation")
      return visitor.template operator()<RichBlockAnimation>();
    if (val == "audio")
      return visitor.template operator()<RichBlockAudio>();
    if (val == "photo")
      return visitor.template operator()<RichBlockPhoto>();
    if (val == "video")
      return visitor.template operator()<RichBlockVideo>();
    if (val == "voice_note")
      return visitor.template operator()<RichBlockVoiceNote>();
    if (val == "thinking")
      return visitor.template operator()<RichBlockThinking>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum RichBlock::type_e;
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
