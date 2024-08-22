#pragma once

#include <cstdint>
#include <memory>
#include <string>
// TODO разобраться откуда лезет #define YIeld и прикрыть эти хедеры (азио)
#ifdef _WIN32
#undef Yield
#endif
#include <kelcoro/channel.hpp>

#include "tgbm/types/Update.h"

namespace tgbm {

class Api;

dd::channel<Update::Ptr> long_poll(Api api, std::int32_t limit, std::chrono::seconds timeout,
                                   std::shared_ptr<std::vector<std::string>> allowUpdates = nullptr);

}  // namespace tgbm
