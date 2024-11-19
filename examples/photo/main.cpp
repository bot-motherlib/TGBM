
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
    // commands handled automatically by bot ('/send_cat')
    (void)u;
  }
}

int main() {
  const char* token = std::getenv("BOT_TOKEN");
  if (!token) {
    fmt::println("launching telegram bot requires bot token from @BotFather");
    return -1;
  }
  tgbm::bot bot{token};
  bot.commands.add("send_cat", [&bot](tgbm::api::Message&& msg) {
    bot.api
        .sendPhoto({
            .chat_id = msg.chat->id,
            .photo = tgbm::api::InputFile::from_file(EXAMPLE_PHOTO_PATH, "image/jpeg"),
        })
        .start_and_detach();
  });
  start_main_task(bot).start_and_detach();
  bot.run();

  return 0;
}
