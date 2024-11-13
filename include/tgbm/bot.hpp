#pragma once

#include <memory>
#include <string>

#include "kelcoro/channel.hpp"

#include "tgbm/api/telegram.hpp"
#include "tgbm/net/http_client.hpp"
#include "tgbm/long_poll.hpp"

namespace tgbm {

struct http_client;

// postcondition: client != nullptr
std::unique_ptr<http_client> default_http_client(std::string_view host);

struct bot_commands {
  using on_command_handler_t = move_only_fn<void(api::Message&& text) const>;

 private:
  struct command {
    on_command_handler_t handler;
    std::string description;
  };
  std::unordered_map<std::string, command> commands;

 public:
  [[nodiscard]] static bool is_valid_name(std::string_view) noexcept;
  // TODO conforming with TG rules (parsing etc) + NOT EMPTY (""), send set commands etc
  // TODO args (separately, parse_command_args<Args...>)
  // replaces current command if exist
  void add(std::string name, on_command_handler_t oncommand, std::string description = {});
  on_command_handler_t::cptr find(std::string_view name) const noexcept;
};

struct bot {
 private:
  // invariant: != nullptr
  std::unique_ptr<http_client> client;
  bool stopped = false;

 public:
  api::telegram api;
  // commands of bot
  // command name must be 1-32 characters. Can contain only lowercase English letters, digits and underscores.
  // Note: commands may trigger only on message/channel_post
  bot_commands commands;
  const_string token;

  // uses default http client
  explicit bot(const_string bottoken, std::string_view host = "api.telegram.org")
      : client(default_http_client(host)), api(*client, bottoken), commands(), token(bottoken) {
  }

  explicit bot(const_string bottoken, std::unique_ptr<http_client> c)
      : client(c ? std::move(c) : default_http_client("api.telegram.org")),
        api(*client, bottoken),
        commands(),
        token(bottoken) {
  }

  std::string_view get_token() const noexcept {
    return token.str();
  }
  std::string_view get_host() const noexcept {
    return client->get_host();
  }

  http_client& get_client() const {
    return *client;
  }

  // 'allowed_updates':
  //    - empty array means almost all arguments
  // Notes:
  //  * there are must be <= 1 getUpdates request for bot globally at one time
  //  * ignores update, which handled by bot.commands already
  dd::channel<api::Update> updates(long_poll_options = {});

  // works until all done or error happens
  void run() {
    stopped = false;
    client->run();
  }

  void stop() {
    stopped = true;
    client->stop();
  }

  // returns true if bot is blocked by user
  dd::task<bool> blocked_by_user(api::Integer chatid, deadline_t = deadline_t::never()) const;
};

// returns http status
dd::task<int> download_file(api::telegram, api::String file_path, on_data_part_fn_ref,
                            deadline_t = deadline_t::never());

// returns http status
dd::task<int> download_file_by_id(api::telegram, api::String fileid, on_data_part_fn_ref,
                                  deadline_t = deadline_t::never());

}  // namespace tgbm
