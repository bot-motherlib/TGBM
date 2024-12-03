
#include <tgbm/bot.hpp>

#include <tgbm/utils/formatters.hpp>
#include <tgbm/utils/scope_exit.hpp>

dd::task<void> start_main_task(tgbm::bot& bot) {
  on_scope_exit {
    // stop bot on failure
    bot.stop();
  };
  fmt::println("launching echobot, info: {}", co_await bot.api.getMe());

  co_foreach(tgbm::api::Update && u, bot.updates()) {
    tgbm::api::Message* m = u.get_message();
    if (!m || !m->text)
      continue;
    fmt::println("user wrote {}", *m->text);
    bot.api.sendMessage({.chat_id = m->chat->id, .text = "your message is: " + m->text.value()})
        .start_and_detach();
  }
}

int main() {
  const char* token = std::getenv("BOT_TOKEN");
  if (!token) {
    fmt::println("launching telegram bot requires bot token from @BotFather");
    return -1;
  }
  tgbm::bot bot{token /*"api.telegram.org", "some_ssl_certificate"*/};

  start_main_task(bot).start_and_detach();
  bot.run();

  return 0;
}
