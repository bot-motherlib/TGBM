#pragma once

#include "tgbm/net/http2/errors.hpp"

#define KELBON_HPACK_HANDLE_PROTOCOL_ERROR \
  throw ::tgbm::http2::hpack_error {       \
  }
#include <hpack/hpack.hpp>
#undef KELBON_HPACK_HANDLE_PROTOCOL_ERROR
