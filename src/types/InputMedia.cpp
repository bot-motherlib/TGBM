#include "tgbm/types/InputMediaPhoto.h"
#include "tgbm/types/InputMediaVideo.h"
#include "tgbm/types/InputMediaAnimation.h"
#include "tgbm/types/InputMediaAudio.h"
#include "tgbm/types/InputMediaDocument.h"

#include <string>

using namespace tgbm;

const std::string InputMediaPhoto::TYPE = "photo";
const std::string InputMediaVideo::TYPE = "video";
const std::string InputMediaAnimation::TYPE = "animation";
const std::string InputMediaAudio::TYPE = "audio";
const std::string InputMediaDocument::TYPE = "document";
