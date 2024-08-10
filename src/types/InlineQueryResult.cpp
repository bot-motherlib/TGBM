#include "tgbm/types/InlineQueryResultCachedAudio.h"
#include "tgbm/types/InlineQueryResultCachedDocument.h"
#include "tgbm/types/InlineQueryResultCachedGif.h"
#include "tgbm/types/InlineQueryResultCachedMpeg4Gif.h"
#include "tgbm/types/InlineQueryResultCachedPhoto.h"
#include "tgbm/types/InlineQueryResultCachedSticker.h"
#include "tgbm/types/InlineQueryResultCachedVideo.h"
#include "tgbm/types/InlineQueryResultCachedVoice.h"
#include "tgbm/types/InlineQueryResultAudio.h"
#include "tgbm/types/InlineQueryResultContact.h"
#include "tgbm/types/InlineQueryResultGame.h"
#include "tgbm/types/InlineQueryResultDocument.h"
#include "tgbm/types/InlineQueryResultLocation.h"
#include "tgbm/types/InlineQueryResultVenue.h"
#include "tgbm/types/InlineQueryResultVoice.h"
#include "tgbm/types/InlineQueryResultArticle.h"
#include "tgbm/types/InlineQueryResultGif.h"
#include "tgbm/types/InlineQueryResultMpeg4Gif.h"
#include "tgbm/types/InlineQueryResultPhoto.h"
#include "tgbm/types/InlineQueryResultVideo.h"

#include <string>

using namespace tgbm;

const std::string InlineQueryResultCachedAudio::TYPE = "audio";
const std::string InlineQueryResultCachedDocument::TYPE = "document";
const std::string InlineQueryResultCachedGif::TYPE = "gif";
const std::string InlineQueryResultCachedMpeg4Gif::TYPE = "mpeg4_gif";
const std::string InlineQueryResultCachedPhoto::TYPE = "photo";
const std::string InlineQueryResultCachedSticker::TYPE = "sticker";
const std::string InlineQueryResultCachedVideo::TYPE = "video";
const std::string InlineQueryResultCachedVoice::TYPE = "voice";

const std::string InlineQueryResultArticle::TYPE = "article";
const std::string InlineQueryResultAudio::TYPE = "audio";
const std::string InlineQueryResultContact::TYPE = "contact";
const std::string InlineQueryResultGame::TYPE = "game";
const std::string InlineQueryResultDocument::TYPE = "document";
const std::string InlineQueryResultGif::TYPE = "gif";
const std::string InlineQueryResultLocation::TYPE = "location";
const std::string InlineQueryResultMpeg4Gif::TYPE = "mpeg4_gif";
const std::string InlineQueryResultPhoto::TYPE = "photo";
const std::string InlineQueryResultVenue::TYPE = "venue";
const std::string InlineQueryResultVideo::TYPE = "video";
const std::string InlineQueryResultVoice::TYPE = "voice";
