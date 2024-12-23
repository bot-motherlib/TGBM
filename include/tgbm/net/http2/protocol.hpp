#pragma once

#include <algorithm>
#include <cstdint>
#include <span>
#include <string_view>
#include <cassert>

#include <hpack/hpack.hpp>
#include <tgbm/net/http2/errors.hpp>
#include <tgbm/utils/memory.hpp>

namespace tgbm {

struct http_request;

}

namespace tgbm::http2 {

enum struct frame_e : uint8_t {
  DATA = 0x0,
  HEADERS = 0x1,
  PRIORITY = 0x2,
  RST_STREAM = 0x3,
  SETTINGS = 0x4,
  PUSH_PROMISE = 0x5,
  PING = 0x6,
  GOAWAY = 0x7,
  WINDOW_UPDATE = 0x8,
  CONTINUATION = 0x9,
  PRIORITY_UPDATE = 0x10,
};

constexpr inline uint32_t initial_window_size_for_connection_overall = uint32_t(65'535);
constexpr inline uint32_t max_window_size = (uint32_t(1) << 31) - 1;
constexpr inline uint32_t frame_header_len = uint32_t(9);
constexpr inline uint32_t frame_len_max = (uint32_t(1) << 24) - 1;

using flags_t = uint8_t;

namespace flags {
// all flags of all frames
constexpr inline flags_t EMPTY_FLAGS = 0;
constexpr inline flags_t ACK = 0x01;          // SETTINGS, PING, means ping answer or settings accept
constexpr inline flags_t END_STREAM = 0x01;   // DATA, HEADERS
constexpr inline flags_t PADDED = 0x08;       // DATA, HEADERS, PUSH_PROMISE
constexpr inline flags_t PRIORITY = 0x20;     // HEADERS (deprecated)
constexpr inline flags_t END_HEADERS = 0x04;  // HEADERS, PUSH_PROMISE

}  // namespace flags

struct frame_header {
  // does not include frame itself
  uint32_t length = 0;
  frame_e type = frame_e(0);
  flags_t flags = flags::EMPTY_FLAGS;
  stream_id_t stream_id = 0;

  template <std::output_iterator<hpack::byte_t> O>
  O send_to(O out) const {
    auto push_byte = [&](uint8_t byte) {
      *out = byte;
      ++out;
    };
    uint32_t len = htonl(length);
    out = std::copy_n(as_bytes(len).data() + 1, 3, out);
    push_byte(uint8_t(type));
    push_byte(flags);
    stream_id_t id = htonl(stream_id);
    out = std::copy_n((uint8_t*)&id, sizeof(id), out);
    return out;
  }

  // precondition: raw_header.size() == http2::frame_header_len
  // not staticaly typed because of std::span ideal interface
  [[nodiscard]] static frame_header parse(std::span<const hpack::byte_t> raw_header) {
    assert(raw_header.size() == frame_header_len);
    frame_header h;
    h.length = (raw_header[0] << 16) | (raw_header[1] << 8) | raw_header[2];
    h.type = frame_e(raw_header[3]);
    h.flags = raw_header[4];
    memcpy(&h.stream_id, raw_header.data() + 5, 4);
    htonli(h.stream_id);
    return h;
  }

  bool operator==(const frame_header&) const = default;
};

// MUST be followed by a SETTINGS frame which MAY be empty
constexpr inline unsigned char connection_preface[] = {
    0x50, 0x52, 0x49, 0x20, 0x2a, 0x20, 0x48, 0x54, 0x54, 0x50, 0x2f, 0x32,
    0x2e, 0x30, 0x0d, 0x0a, 0x0d, 0x0a, 0x53, 0x4d, 0x0d, 0x0a, 0x0d, 0x0a,
};

struct data_frame {
  /*
    <header>
    [Pad Length (8)],
    Data (..),
    Padding (..2040),
  */
};

// opens new stream
struct headers_frame {
  /*
    <header>
    [Pad Length (8)],
    [Exclusive (1)],          //
    [Stream Dependency (31)], //
    [Weight (8)],             // all deprecated
    Field Block Fragment (..),
    Padding (..2040),
  */

  // do not have 'parse', its in http2_client (because requires decoder, padding remove etc)
};

// DEPRECATED
struct priority_frame {
  // not handled
};

// terminates stream
struct rst_stream {
  frame_header header;
  errc_e error_code;

  static constexpr inline size_t len = frame_header_len + 4;

  template <std::output_iterator<byte_t> O>
  static O form(stream_id_t streamid, errc_e ec, O out) {
    assert(streamid != 0);
    frame_header header{
        .length = 4,
        .type = frame_e::RST_STREAM,
        .flags = flags::EMPTY_FLAGS,
        .stream_id = streamid,
    };
    out = header.send_to(out);
    htonli(ec);
    return std::copy_n(as_bytes(ec).data(), 4, out);
  }

  [[nodiscard]] static rst_stream parse(frame_header h, std::span<const byte_t> bytes) {
    assert(h.type == frame_e::RST_STREAM);
    if (h.stream_id == 0 || h.length != 4)
      throw protocol_error{};
    rst_stream frame(h);
    memcpy(&frame.error_code, bytes.data(), 4);
    htonli(frame.error_code);
    return frame;
  }
};

enum : uint16_t {
  SETTINGS_HEADER_TABLE_SIZE = 0x1,
  SETTINGS_ENABLE_PUSH = 0x2,
  SETTINGS_MAX_CONCURRENT_STREAMS = 0x3,
  SETTINGS_INITIAL_WINDOW_SIZE = 0x4,
  SETTINGS_MAX_FRAME_SIZE = 0x5,
  SETTINGS_MAX_HEADER_LIST_SIZE = 0x6,
  // extension https://datatracker.ietf.org/doc/html/rfc9218
  SETTINGS_NO_RFC7540_PRIORITIES = 0x9,
};

struct settings_t {
  static constexpr inline uint32_t max_max_concurrent_streams = ((uint32_t(1) << 31) - 1);

  uint32_t header_table_size = 4096;
  bool enable_push = false;
  uint32_t max_concurrent_streams = max_max_concurrent_streams;
  // only for stream-level size!
  uint32_t initial_stream_window_size = 65'535;
  uint32_t max_frame_size = 16'384;
  uint32_t max_header_list_size = uint32_t(-1);
  // https://datatracker.ietf.org/doc/html/rfc9218
  bool deprecated_priority_disabled = false;
};

#pragma pack(push, 1)

struct [[gnu::packed]] setting_t {
  uint16_t identifier;
  uint32_t value;

  template <std::output_iterator<byte_t> O>
  static O form(setting_t s, O out) {
    s.identifier = htonl(s.identifier);
    s.value = htonl(s.value);
    return std::copy_n(as_bytes(s).data(), sizeof(s), out);
  }

  [[nodiscard]] static setting_t parse(std::span<const byte_t, 6> bytes) noexcept {
    setting_t s;
    memcpy(&s, bytes.data(), sizeof(s));
    s.identifier = htonl(s.identifier);
    s.value = htonl(s.value);
    return s;
  }
};
static_assert(sizeof(setting_t) == 6);

#pragma pack(pop)

// fills settings while parsing 'setting_t' one by one
// client side
struct server_settings_visitor {
  settings_t& settings;  // must be server settings
  uint32_t header_table_size_decrease = 0;

  constexpr void operator()(setting_t s) {
    switch (s.identifier) {
      case SETTINGS_HEADER_TABLE_SIZE:
        if (settings.header_table_size > s.value)
          header_table_size_decrease = settings.header_table_size - s.value;
        settings.header_table_size = s.value;
        return;
      case SETTINGS_ENABLE_PUSH:
        if (s.value > 0)
          throw protocol_error{};  // server MUST NOT send i
        settings.enable_push = s.value;
        return;
      case SETTINGS_MAX_CONCURRENT_STREAMS:
        settings.max_concurrent_streams = s.value;
        return;
      case SETTINGS_INITIAL_WINDOW_SIZE:
        settings.initial_stream_window_size = s.value;
        return;
      case SETTINGS_MAX_FRAME_SIZE:
        settings.max_frame_size = s.value;
        return;
      case SETTINGS_MAX_HEADER_LIST_SIZE:
        settings.max_header_list_size = s.value;
        return;
      case SETTINGS_NO_RFC7540_PRIORITIES:
        /*
        Senders MUST NOT change the SETTINGS_NO_RFC7540_PRIORITIES value after the first SETTINGS frame.
        Receivers that detect a change MAY treat it as a connection error of type PROTOCOL_ERROR.
        */
        if (s.value > 1 || settings.deprecated_priority_disabled != s.value)
          throw protocol_error{};
        return;
      default:
          // ignore if dont know
          ;
    }
  };
};

// fills settings while parsing 'setting_t' one by one
// server side
struct client_settings_visitor {
  settings_t& settings;  // must be client settings
  uint32_t header_table_size_decrease = 0;

  constexpr void operator()(setting_t s) {
    switch (s.identifier) {
      case SETTINGS_HEADER_TABLE_SIZE:
        if (settings.header_table_size > s.value)
          header_table_size_decrease = settings.header_table_size - s.value;
        settings.header_table_size = s.value;
        return;
      case SETTINGS_ENABLE_PUSH:
        if (s.value > 1)
          throw protocol_error{};
        settings.enable_push = s.value;
        return;
      case SETTINGS_MAX_CONCURRENT_STREAMS:
        settings.max_concurrent_streams = s.value;
        return;
      case SETTINGS_INITIAL_WINDOW_SIZE:
        settings.initial_stream_window_size = s.value;
        return;
      case SETTINGS_MAX_FRAME_SIZE:
        settings.max_frame_size = s.value;
        return;
      case SETTINGS_MAX_HEADER_LIST_SIZE:
        settings.max_header_list_size = s.value;
        return;
      case SETTINGS_NO_RFC7540_PRIORITIES:
        /*
        Senders MUST NOT change the SETTINGS_NO_RFC7540_PRIORITIES value after the first SETTINGS frame.
        Receivers that detect a change MAY treat it as a connection error of type PROTOCOL_ERROR.
        */
        if (s.value > 1 || settings.deprecated_priority_disabled != s.value)
          throw protocol_error{};
        return;
      default:
          // ignore if dont know
          ;
    }
  };
};

struct settings_frame {
  frame_header header;

  /*
    <header>
    Setting (48) ...,

    Setting {
      Identifier (16),
      Value (32),
    }
  */

  template <std::output_iterator<hpack::byte_t> O>
  static O form(const settings_t& settings, O out) noexcept {
    static constexpr settings_t default_;

    // calculate len

    uint32_t len = 0;
    len += settings.header_table_size != default_.header_table_size;
    len += settings.enable_push != default_.enable_push;
    len += settings.max_concurrent_streams != default_.max_concurrent_streams;
    len += settings.initial_stream_window_size != default_.initial_stream_window_size;
    len += settings.max_frame_size != default_.max_frame_size;
    len += settings.max_header_list_size != default_.max_header_list_size;
    len *= sizeof(setting_t);

    // send frame header

    frame_header header{
        .length = len,
        .type = frame_e::SETTINGS,
        .flags = 0,
        .stream_id = 0,  // connection related
    };
    out = header.send_to(out);

    // send settings VLA

    auto insert_setting = [&](setting_t s) { out = setting_t::form(s, out); };
#define $PUSH_SETTING(NAME, ENUM_NAME) \
  if (settings.NAME != default_.NAME)  \
  insert_setting({ENUM_NAME, settings.NAME})

    $PUSH_SETTING(header_table_size, SETTINGS_HEADER_TABLE_SIZE);
    $PUSH_SETTING(enable_push, SETTINGS_ENABLE_PUSH);
    $PUSH_SETTING(max_concurrent_streams, SETTINGS_MAX_CONCURRENT_STREAMS);
    $PUSH_SETTING(initial_stream_window_size, SETTINGS_INITIAL_WINDOW_SIZE);
    $PUSH_SETTING(max_frame_size, SETTINGS_MAX_FRAME_SIZE);
    $PUSH_SETTING(max_header_list_size, SETTINGS_MAX_HEADER_LIST_SIZE);
#undef $PUSH_SETTING

    return out;
  }

  // ordering matters, must be handled in order they received
  static void parse(frame_header header, std::span<const byte_t> bytes, auto&& setting_visitor) {
    assert(header.type == frame_e::SETTINGS);
    if (header.stream_id != 0 || (bytes.size() % sizeof(setting_t)) != 0)
      throw protocol_error{};
    setting_t s;
    for (auto b = bytes.begin(); b != bytes.end(); b += 6) {
      s = setting_t::parse(std::span<const byte_t, 6>{b, b + 6});
      setting_visitor(s);
    }
  }
};

// consists only of frame header
constexpr frame_header accepted_settings_frame() noexcept {
  return frame_header{
      .length = 0,
      .type = frame_e::SETTINGS,
      .flags = flags::ACK,
      .stream_id = 0,  // connection related
  };
}

struct push_promise_frame {
  // not handle it
};

// if ACK not setted, requires ping back
struct ping_frame {
  frame_header header;
  byte_t data[8] = {};

  [[nodiscard]] constexpr uint64_t get_data() noexcept {
    return std::bit_cast<uint64_t>(data);
  }

  static constexpr inline size_t len = frame_header_len + 8;

  template <std::output_iterator<byte_t> O>
  static O form(uint64_t data, bool request_answer, O out) {
    frame_header h{
        .length = 8,
        .type = frame_e::PING,
        .flags = request_answer ? flags_t(0) : flags::ACK,
        .stream_id = 0,
    };
    out = h.send_to(out);
    return std::copy_n((char*)&data, 8, out);
  }

  [[nodiscard]] static ping_frame parse(frame_header h, std::span<const byte_t> bytes) {
    assert(h.type == frame_e::PING && h.length == bytes.size());
    ping_frame f(h);
    if (h.stream_id != 0 || h.length != 8)
      throw protocol_error{};
    memcpy(f.data, bytes.data(), 8);
    return f;
  }
};

// initiates shutdown on connection.
struct goaway_frame {
  frame_header header;
  stream_id_t last_stream_id;
  errc_e error_code;
  std::string debug_info;
  /*
    <header>
    Reserved (1),
    Last-Stream-ID (31),
    Error Code (32),
    Additional Debug Data (..),
  */

  static goaway_frame parse(frame_header header, std::span<const byte_t> bytes) {
    assert(header.type == frame_e::GOAWAY);
    goaway_frame frame;
    if (header.length < 8)
      throw protocol_error{};
    memcpy(&frame.last_stream_id, bytes.data(), 4);
    memcpy(&frame.error_code, bytes.data() + 4, 4);
    htonli(frame.last_stream_id);
    htonli(frame.error_code);
    frame.debug_info =
        std::string_view((const char*)bytes.data() + 8, (const char*)bytes.data() + bytes.size());
    return frame;
  }

  [[noreturn]] static void parse_and_throw_goaway(frame_header header, std::span<const byte_t> bytes) {
    goaway_frame f = parse(header, bytes);
    throw goaway_exception(f.last_stream_id, f.error_code, std::move(f.debug_info));
  }

  template <std::output_iterator<byte_t> O>
  static O form(stream_id_t last_stream_id, errc_e error_code, std::string debug_info, O out) {
    out =
        frame_header{
            .length = 8 + uint32_t(debug_info.size()),
            .type = frame_e::GOAWAY,
            .flags = 0,
            .stream_id = 0,
        }
            .send_to(out);
    htonli(last_stream_id);
    htonli(error_code);
    out = std::copy_n(as_bytes(last_stream_id).data(), 4, out);
    out = std::copy_n(as_bytes(error_code).data(), 4, out);
    return std::copy_n(debug_info.data(), debug_info.size(), out);
  }
};

// window size applicable only to DATA frames
struct window_update_frame {
  frame_header header;
  uint32_t window_size_increment;
  /*
    <header>
    Reserved (1),
    Window Size Increment (31),
  */
  static constexpr inline size_t len = frame_header_len + 4;

  // id == 0 for connection-wide
  template <std::output_iterator<byte_t> O>
  static O form(stream_id_t id, uint32_t increment, O out) {
    assert(increment != 0);  // not valid by RFC
    out =
        frame_header{
            .length = 4,
            .type = frame_e::WINDOW_UPDATE,
            .flags = flags::EMPTY_FLAGS,
            .stream_id = id,
        }
            .send_to(out);
    htonli(increment);
    return std::copy_n(as_bytes(increment).data(), sizeof(increment), out);
  }

  [[nodiscard]] static window_update_frame parse(frame_header header, std::span<const byte_t> bytes) {
    assert(header.length == bytes.size());
    if (header.length != 4)
      throw connection_error(errc_e::FRAME_SIZE_ERROR);
    window_update_frame frame{.header = header};
    std::memcpy(&frame.window_size_increment, bytes.data(), 4);
    htonli(frame.window_size_increment);
    if (frame.window_size_increment >= (1 << 31))
      throw protocol_error{};  // reserved bit filled
    if (frame.window_size_increment == 0) {
      if (header.stream_id)
        throw stream_error{header.stream_id};
      else
        throw protocol_error{};
    }
    return frame;
  }
};

struct continuation_frame {
  // not handle it
};

// extension https://www.rfc-editor.org/rfc/rfc9218#name-the-priority_update-frame
struct priority_update_frame {
  // not handle it
};

template <std::output_iterator<hpack::byte_t> O>
static O form_connection_initiation(settings_t settings, O out) {
  out = std::copy_n(connection_preface, sizeof(connection_preface), out);
  return settings_frame::form(settings, out);
}

// throws on protocol errors
inline void increment_window_size(uint32_t& size, uint32_t window_size_increment) {
  if (window_size_increment == 0)
    throw protocol_error{};
  // avoid overflow
  if (uint64_t(size) + uint64_t(window_size_increment) > uint64_t(http2::max_window_size))
    throw protocol_error{};
  size += window_size_increment;
}

// removes padding for DATA/HEADERS with PADDED flag
inline bool strip_padding(std::span<byte_t>& bytes) {
  if (bytes.empty())
    return false;
  int padlen = bytes[0];
  if (padlen + 1 > bytes.size())
    return false;
  remove_prefix(bytes, 1);
  remove_suffix(bytes, padlen);
  return true;
}

// fills 'req' excluding 'body', handles duplicate pseudoheaders / incorrect ordering
// returns span of unparsed headers
// its caller responsibility to check correctness of parsed values
void parse_http2_request_headers(hpack::decoder& d, std::span<const hpack::byte_t> bytes, http_request& req);

}  // namespace tgbm::http2
