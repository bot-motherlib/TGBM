
#include <tgbm/bot.hpp>

#include <tgbm/utils/formatters.hpp>
#include <zal/zal.hpp>

static tgbm::api::reply_markup_t make_inline_keyboard() {
  using namespace tgbm::api;
  using button = InlineKeyboardButton;

  reply_markup_t markup;
  auto& m = markup.emplace<InlineKeyboardMarkup>();

  m.inline_keyboard = {
      {{.text = "Button 1", .data = button::callback_data{.value = "yes"}},
       {.text = "Button 2", .data = button::callback_data{.value = "no"}}},
  };
  return markup;
}

static dd::task<void> answer_query(tgbm::bot& bot, tgbm::api::CallbackQuery q) {
  using namespace tgbm::api;

  bool success = co_await bot.api.answerCallbackQuery({.callback_query_id = q.id, .text = q.data});
  if (!success) {
    fmt::println(stderr, "bot cannot handle callback query =(");
    co_return;
  }

  int_or_str id;
  if (q.message)
    id = q.message->chat->id;

  Message sended = co_await bot.api.sendMessage({
      .chat_id = id,
      .text = fmt::format("your answer is {}", q.data.value_or("<nothing>")),
  });
  fmt::println("user receives: {}", sended);
}

dd::task<void> start_main_task(tgbm::bot& bot) {
  on_scope_exit {
    // stop bot on failure
    bot.stop();
  };
  fmt::println("launching echobot, info: {}", co_await bot.api.getMe());

  co_foreach(tgbm::api::Update && u, bot.updates()) {
    tgbm::api::CallbackQuery* cq = u.get_callback_query();
    if (cq)
      answer_query(bot, std::move(*cq)).start_and_detach();
  }
}

int main() {
  const char* token = std::getenv("BOT_TOKEN");
  if (!token) {
    fmt::println("launching telegram bot requires bot token from @BotFather");
    return -1;
  }

  tgbm::bot bot{token /*"api.telegram.org", "some_ssl_certificate"*/};

  bot.commands.add("get_keyboard", [&bot](tgbm::api::Message&& m) {
    dd::task sendmsg = bot.api.sendMessage({
        .chat_id = m.chat->id,
        .text = "yes or no?",
        .reply_markup = make_inline_keyboard(),
    });
    sendmsg.start_and_detach();
  });

  start_main_task(bot).start_and_detach();
  bot.run();

  return 0;
}
