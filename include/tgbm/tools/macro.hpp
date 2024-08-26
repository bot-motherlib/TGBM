#pragma once

#include "kelcoro/noexport/macro.hpp"

#ifdef __clang__
#define TGBM_TRIVIAL_ABI [[clang::trivial_abi]]
#else
#define TGBM_TRIVIL_ABI
#endif
