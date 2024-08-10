#pragma once

#include "tgbm/EventBroadcaster.h"
#include "tgbm/types/Update.h"
#include "tgbm/tools/StringTools.h"

#include <algorithm>
#include <cstddef>
#include <string>

namespace tgbm {

class TGBOT_API EventHandler {
 public:
  explicit EventHandler(const EventBroadcaster& broadcaster) : _broadcaster(broadcaster) {
  }

  void handleUpdate(const Update::Ptr& update) const;

 private:
  const EventBroadcaster& _broadcaster;

  void handleMessage(const Message::Ptr& message) const;
};

}  // namespace tgbm
