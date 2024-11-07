#pragma once

#include <anyany/anyany.hpp>

namespace tgbm {

template <typename Signature>
using fn_ref = aa::ref<aa::call<Signature>>;

template <typename Signature>
using fn_cref = aa::cref<aa::call<Signature>>;

template <typename Signature>
using fn_ptr = aa::ptr<aa::call<Signature>>;

template <typename Signature>
using fn_cptr = aa::cptr<aa::call<Signature>>;

template <typename Signature>
using move_only_fn = aa::any_with<aa::call<Signature>, aa::move>;

}  // namespace tgbm
