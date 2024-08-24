
#include <string>
#include <vector>
#include <optional>  // TODO own optional

namespace tgbm::api {

using String = std::string;
using Integer = int64_t;
using Boolean = bool;
using True = std::true_type;

template <typename T>
using arrayof = std::vector<T>;

template <typename T>
using optional = std::optional<T>;

}  // namespace tgbm::api
