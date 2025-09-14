#pragma once

#include <http2/http_body.hpp>
#include "http2/errors.hpp"
#include "http2/http2_client_options.hpp"
#include "http2/http_base.hpp"
#include "http2/utils/deadline.hpp"

namespace tgbm {

using http2::byte_t;
using http2::deadline_after;
using http2::deadline_t;
using http2::duration_t;
using http2::e2str;
using http2::enum_from_string;
using http2::http2_client_options;
using http2::http_header_t;
using http2::http_method_e;
using http2::http_request;
using http2::http_response;
using http2::io_error_code;
using http2::network_exception;
using http2::on_data_part_fn_ptr;
using http2::on_header_fn_ptr;
using http2::reqerr_e;
using http2::scheme_e;
using http2::timeout_exception;
using on_data_part_fn_ref = decltype(*on_data_part_fn_ptr{});

// does nothing for good status, rethrows error (timeout, http_exception etc) for bad status
void handle_telegram_http_status(int status);

}  // namespace tgbm
