#include <tgbm/bot.hpp>

#include <tgbm/net/http2/client.hpp>

namespace tgbm {

static std::unique_ptr<http_client> make_http2_client(bot_options options) {
  http2_client_init init;
  init.host = options.host;
  init.dst = options.dst;
  init.dstport = options.dstport;
  if (!options.additional_ssl_cert.empty()) {
    init.tcp_options.additional_ssl_certificates.push_back(std::move(options.additional_ssl_cert));
    init.tcp_options.host_for_name_verification.emplace(options.host);
  }
  init.options.logctx = std::move(options.logctx);
  init.starter = std::move(options.starter);
  return std::unique_ptr<http_client>(new http2_client(std::move(init)));
}

std::unique_ptr<http_client> default_http_client(std::string_view host,
                                                 std::filesystem::path additional_ssl_cert,
                                                 http2::log_context logctx) {
  bot_options opts;
  opts.host = host;
  opts.dst = host;
  opts.dstport = 443;
  opts.additional_ssl_cert = std::move(additional_ssl_cert);
  opts.logctx = std::move(logctx);
  opts.starter = {};
  return make_http2_client(std::move(opts));
}

bot::bot(std::string bottoken, bot_options options)
    : client(make_http2_client(std::move(options))), api(*client, bottoken), commands(), token(bottoken) {
}

void bot_commands::add(std::string name, on_command_handler_t oncommand, std::string description) {
  commands.insert_or_assign(std::move(name), command(std::move(oncommand), std::move(description)));
}

bot_commands::on_command_handler_t::cptr bot_commands::find(std::string_view name) const noexcept {
  // TODO transparent search
  auto it = commands.find(std::string(name));
  if (it == commands.end())
    return nullptr;
  return &it->second.handler;
}

static constexpr std::string_view parse_command(std::string_view text) {
  // rm whitespaces
  while (!text.empty()) {
    char arr[] = " \n\t";
    if (std::ranges::find(arr, text.front()) == std::end(arr))
      break;
    text.remove_prefix(1);
  }
  if (text.empty() || text.front() != '/')
    return "";
  text.remove_prefix(1);
  auto index = text.find_first_of(" \n\t");
  return text.substr(0, index);
}

static_assert(parse_command("fdsfsf") == "");
static_assert(parse_command("/start") == "start");
static_assert(parse_command("   \t\n/start") == "start");
static_assert(parse_command("/start arg1 arg2") == "start");
static_assert(parse_command("  \n/command  ") == "command");

dd::channel<api::Update> bot::updates(long_poll_options options) {
  dd::channel chan = long_poll(api, std::move(options));

  auto maybe_handle_command = [&](api::Message& m) {
    if (!m.text)
      return false;
    std::string_view command = parse_command(*m.text);
    if (command.empty())
      return false;
    auto handler = commands.find(command);
    if (!handler)
      return false;
    (*handler)(std::move(m));
    return true;
  };
  co_foreach(api::Update && u, chan) {
    switch (u.type()) {
      case api::Update::type_e::k_channel_post:
        if (!maybe_handle_command(*u.get_channel_post()))
          co_yield std::move(u);
        break;
      case api::Update::type_e::k_message:
        if (!maybe_handle_command(*u.get_message()))
          co_yield std::move(u);
        break;
      default:
        co_yield std::move(u);
        break;
    }
    if (stopped)
      co_return;
  }
}

dd::task<int> download_file(api::telegram api, api::String file_path, on_data_part_fn_ref on_data_part,
                            deadline_t deadline) {
  int status = co_await api.client.send_request(
      nullptr, &on_data_part,
      http_request{.authority = {},
                   .path = fmt::format("/file/bot{}/{}", api.bottoken.str(), file_path),
                   .method = http_method_e::GET,
                   .scheme = scheme_e::HTTPS},
      deadline);
  co_return status;
}

dd::task<int> download_file_by_id(
    api::telegram api, api::String fileid,
    fn_ref<void(std::span<const http2::byte_t>, bool is_last_chunk)> on_data_part, deadline_t deadline) {
  api::File info = co_await api.getFile({.file_id = std::move(fileid)}, deadline);
  if (!info.file_path)
    co_return 404;
  co_return co_await download_file(std::move(api), std::move(*info.file_path), on_data_part, deadline);
}

dd::task<bool> bot::blocked_by_user(api::Integer chatid, deadline_t deadline) const {
  try {
    (void)co_await api.sendChatAction({.chat_id = chatid, .action = "typing"});
    co_return false;
  } catch (std::exception& e) {
    co_return std::string_view("Forbidden: bot was blocked by the user") == e.what();
  }
}

}  // namespace tgbm
