#if 0
  #pragma once

  #include "tgbm/EventBroadcaster.h"
  #include "tgbm/types/Update.h"
  #include "tgbm/tools/StringTools.h"

  #include <algorithm>
  #include <cstddef>
  #include <string>

  #include "tgbm/api/types/Update.hpp"
  #include "tgbm/api/types/Message.hpp"

namespace tgbm {

class EventHandler {
 public:
  explicit EventHandler(const EventBroadcaster& broadcaster) : _broadcaster(broadcaster) {
  }

  void handleUpdate(const api::Update& update) const;

 private:
  const EventBroadcaster& _broadcaster;

  void handleMessage(const api::Message& message) const;
};

}  // namespace tgbm
#endif
