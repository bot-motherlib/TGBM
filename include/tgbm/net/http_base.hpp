#pragma once

#include <hidi/http_body.hpp>
#include <hidi/errors.hpp>
#include <hidi/h2client_options.hpp>
#include <hidi/http_base.hpp>
#include <hidi/utils/deadline.hpp>

namespace tgbm {

using hidi::byte_t;
using hidi::e2str;
using hidi::efromstr;
using http2_client_options = hidi::h2client_options;
using hidi::http_header_t;
using hidi::http_method_e;
using hidi::http_request;
using hidi::http_response;
using hidi::io_error_code;
using hidi::network_exception;
using hidi::on_data_part_fn_ptr;
using hidi::on_header_fn_ptr;
using hidi::reqerr_e;
using hidi::scheme_e;
using hidi::timeout_exception;
using on_data_part_fn_ref = decltype(*on_data_part_fn_ptr{});

// does nothing for good status, rethrows error (timeout, http_exception etc) for bad status
void handle_telegram_http_status(int status);

}  // namespace tgbm
