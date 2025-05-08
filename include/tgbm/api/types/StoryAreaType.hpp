#pragma once

#include <tgbm/api/types/all_fwd.hpp>
#include <tgbm/api/types/StoryAreaTypeLocation.hpp>
#include <tgbm/api/types/StoryAreaTypeSuggestedReaction.hpp>
#include <tgbm/api/types/StoryAreaTypeLink.hpp>
#include <tgbm/api/types/StoryAreaTypeWeather.hpp>
#include <tgbm/api/types/StoryAreaTypeUniqueGift.hpp>

namespace tgbm::api {

/*Describes the type of a clickable area on a story. Currently, it can be one of*/
struct StoryAreaType {
  oneof<StoryAreaTypeLocation, StoryAreaTypeSuggestedReaction, StoryAreaTypeLink, StoryAreaTypeWeather,
        StoryAreaTypeUniqueGift>
      data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_location,
    k_suggestedreaction,
    k_link,
    k_weather,
    k_uniquegift,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  StoryAreaTypeLocation* get_location() noexcept {
    return data.get_if<StoryAreaTypeLocation>();
  }
  const StoryAreaTypeLocation* get_location() const noexcept {
    return data.get_if<StoryAreaTypeLocation>();
  }
  StoryAreaTypeSuggestedReaction* get_suggestedreaction() noexcept {
    return data.get_if<StoryAreaTypeSuggestedReaction>();
  }
  const StoryAreaTypeSuggestedReaction* get_suggestedreaction() const noexcept {
    return data.get_if<StoryAreaTypeSuggestedReaction>();
  }
  StoryAreaTypeLink* get_link() noexcept {
    return data.get_if<StoryAreaTypeLink>();
  }
  const StoryAreaTypeLink* get_link() const noexcept {
    return data.get_if<StoryAreaTypeLink>();
  }
  StoryAreaTypeWeather* get_weather() noexcept {
    return data.get_if<StoryAreaTypeWeather>();
  }
  const StoryAreaTypeWeather* get_weather() const noexcept {
    return data.get_if<StoryAreaTypeWeather>();
  }
  StoryAreaTypeUniqueGift* get_uniquegift() noexcept {
    return data.get_if<StoryAreaTypeUniqueGift>();
  }
  const StoryAreaTypeUniqueGift* get_uniquegift() const noexcept {
    return data.get_if<StoryAreaTypeUniqueGift>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return string_switch<type_e>(val)
        .case_("location", type_e::k_location)
        .case_("suggested_reaction", type_e::k_suggestedreaction)
        .case_("link", type_e::k_link)
        .case_("weather", type_e::k_weather)
        .case_("unique_gift", type_e::k_uniquegift)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "location")
      return visitor.template operator()<StoryAreaTypeLocation>();
    if (val == "suggested_reaction")
      return visitor.template operator()<StoryAreaTypeSuggestedReaction>();
    if (val == "link")
      return visitor.template operator()<StoryAreaTypeLink>();
    if (val == "weather")
      return visitor.template operator()<StoryAreaTypeWeather>();
    if (val == "unique_gift")
      return visitor.template operator()<StoryAreaTypeUniqueGift>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum StoryAreaType::type_e;
    switch (type()) {
      case k_location:
        return "location";
      case k_suggestedreaction:
        return "suggested_reaction";
      case k_link:
        return "link";
      case k_weather:
        return "weather";
      case k_uniquegift:
        return "unique_gift";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
