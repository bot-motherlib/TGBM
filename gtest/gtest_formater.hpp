#pragma once

#include <iterator>
#include <tgbm/tools/formatters.hpp>

namespace tgbm::api{

template <tgbm::api_type T>
void PrintTo(const T& t, std::ostream* os){
    fmt::format_to(std::ostreambuf_iterator(*os), "{}", t);
}

}
