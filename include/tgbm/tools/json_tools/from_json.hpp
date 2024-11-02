#pragma once 

#include "tgbm/tools/json_tools/json_traits.hpp"
#include "tgbm/tools/json_tools/parse_dom/basic.hpp"

namespace tgbm::json{
    template <typename T, json_dom Json>
    void from_json(const Json& j, T& out) {
        parse_dom::parser<T>:: template parse<Json, default_traits<Json>>(j, out);
    }

    template <typename T>
    T from_json(const json_dom auto& j) {
        T out;
        from_json(j, out);
        return out;
    }
}