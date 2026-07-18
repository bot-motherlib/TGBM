#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*At most one of the optional fields can be present in any given object.*/
struct PollMedia {
  /* Optional. Media is an animation, information about the animation */
  box<Animation> animation;
  /* Optional. Media is an audio file, information about the file; currently, can't be received in a poll
   * option */
  box<Audio> audio;
  /* Optional. Media is a general file, information about the file; currently, can't be received in a poll
   * option */
  box<Document> document;
  /* Optional. The HTTP link attached to the poll option */
  box<Link> link;
  /* Optional. Media is a live photo, information about the live photo */
  box<LivePhoto> live_photo;
  /* Optional. Media is a shared location, information about the location */
  box<Location> location;
  /* Optional. Media is a photo, available sizes of the photo */
  optional<arrayof<PhotoSize>> photo;
  /* Optional. Media is a sticker, information about the sticker; currently, for poll options only */
  box<Sticker> sticker;
  /* Optional. Media is a venue, information about the venue */
  box<Venue> venue;
  /* Optional. Media is a video, information about the video */
  box<Video> video;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
