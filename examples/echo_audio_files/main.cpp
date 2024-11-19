
#include <tgbm/bot.hpp>

#include <tgbm/utils/formatters.hpp>
#include <tgbm/utils/scope_exit.hpp>

#include <fstream>
#include <filesystem>

dd::task<void> echo_audio(tgbm::bot& bot, int64_t chatid, std::string fileid) {
  auto temppath = std::filesystem::temp_directory_path() / (fmt::to_string(chatid) + fileid);
  on_scope_exit {
    std::filesystem::remove(temppath);
  };

  // download audio file
  {
    std::fstream file(temppath, std::ios_base::out | std::ios_base::binary);
    auto write_into_file = [&](std::span<const tgbm::byte_t> bytes, bool /*is last chunk*/) {
      file.write((const char*)bytes.data(), bytes.size());
    };
    int status = co_await tgbm::download_file_by_id(bot.api, fileid, write_into_file);
    if (status < 0) {
      fmt::println(stderr, "cannot send audio, downloading failed");
      co_return;
    }
    if (status == 404) {
      fmt::println(stderr, "file not found");
      co_return;
    }
  }

  // send it back

  tgbm::api::Message msg = co_await bot.api.sendVoice(
      {.chat_id = chatid, .voice = tgbm::api::InputFile::from_file(temppath, "audio/ogg")});

  fmt::println("echo audio message: {}", msg);
}

dd::task<void> start_main_task(tgbm::bot& bot) {
  on_scope_exit {
    // stop bot on failure
    bot.stop();
  };
  fmt::println("launching echobot, info: {}", co_await bot.api.getMe());

  co_foreach(tgbm::api::Update && u, bot.updates()) {
    tgbm::api::Message* m = u.get_message();
    if (!m || !m->voice)
      continue;
    echo_audio(bot, m->chat->id, m->voice->file_id).start_and_detach();
  }
}

int main() {
  const char* token = std::getenv("BOT_TOKEN");
  if (!token) {
    fmt::println("launching telegram bot requires bot token from @BotFather");
    return -1;
  }
  tgbm::bot bot{token};

  start_main_task(bot).start_and_detach();
  bot.run();

  return 0;
}
