
#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>

#include <tgbm/logger.hpp>
#include <tgbm/net/http2/client.hpp>
#include <tgbm/bot.hpp>
#include <tgbm/net/http_client_with_retry.hpp>
#include <tgbm/net/asio_tls_transport.hpp>

dd::task<void> get_bot_info(tgbm::bot& b) {
  auto info = co_await b.api.getMe();

  TGBM_LOG_INFO("NEW API: user: {} ({})", info.first_name, info.username.value_or("<nousername>"));
}

dd::task<void> download_audio(tgbm::bot& bot, int64_t chatid, std::string fileid) {
  auto p = std::filesystem::temp_directory_path() / "someaudio";
  std::fstream file(p, std::ios_base::out | std::ios_base::binary);
  auto write_into_file = [&](std::span<const tgbm::byte_t> bytes, bool) {
    file.write((const char*)bytes.data(), bytes.size());
  };
  int status = co_await tgbm::download_file_by_id(bot.api, fileid, write_into_file);
  file.flush();
  file.close();
  if (status < 0) {
    TGBM_LOG_DEBUG("CANNOT SEND AUDIO BCS OF FILE GET BY ID ERROR");
    co_return;
  }
  if (status == 404) {
    TGBM_LOG_ERROR("FILE NOT FOUND");
    co_return;
  }
  auto msg = co_await bot.api.sendVoice(
      {.chat_id = chatid, .voice = tgbm::api::InputFile::from_file(p.string(), "audio/ogg")});
  std::filesystem::remove(p);
}

dd::job start_main_task(tgbm::bot& bot) try {
  co_foreach(tgbm::api::Update && u, bot.updates({.allowed_updates = tgbm::api::allowed_updates::almost_all(),
                                                  .drop_pending_updates = true})) {
    tgbm::api::Message* m = u.get_message();
    TGBM_LOG_DEBUG("received update, id: {}", u.update_id.value);
    if (!m)
      continue;
    if (m->voice)
      download_audio(bot, m->chat->id, m->voice->file_id).start_and_detach();
    else {
      fmt::println("user wrote {}", m->text.value_or("<notext>"));
      bot.api.sendMessage({.chat_id = m->chat->id, .text = "your message is: " + m->text.value()})
          .start_and_detach();
    }
  }
  TGBM_LOG_INFO("main task ended, no errors");
} catch (std::exception& e) {
  TGBM_LOG_ERROR("main task ended with error: {}", e.what());
  bot.stop();
}

static tgbm::api::InputFile test_big_file() {
  tgbm::api::InputFile f;
  f.filename = "myname";
  f.mimetype = "text/plain";
  // 15 MB of 'a'
  f.data = std::string(1024 * 1024 * 15, 'a');
  return f;
}

static tgbm::api::InputFile test_small_file() {
  tgbm::api::InputFile f;
  f.filename = "myname";
  f.mimetype = "text/plain";
  // 512 byte of 'b'
  f.data = std::string(512, 'b');
  return f;
}

int main() try {
  fmt::println("{}", __DATE__);
  const char* token = std::getenv("BOT_TOKEN");
  if (!token) {
    fmt::println("launching telegram bot requires bot token from @BotFather");
    return -1;
  }

  auto* raw_client = new tgbm::http_client_with_retries<tgbm::http2_client>("api.telegram.org");
  raw_client->wait_between_retries = std::chrono::seconds(1);
  raw_client->retry_count = 4;

  tgbm::bot bot(token, std::unique_ptr<tgbm::http_client>(raw_client));

  assert(bot.get_token() == token);
  bot.commands.add("stop", [&bot](tgbm::api::Message&&) { bot.stop(); });
  bot.commands.add("send_file", [&bot](const tgbm::api::Message message) {
    bot.api.sendDocument({.chat_id = message.chat->id, .document = test_small_file()}).start_and_detach();
  });
  bot.commands.add("send_big_file", [&bot](tgbm::api::Message&& message) {
    bot.api.sendDocument({.chat_id = message.chat->id, .document = test_big_file()}).start_and_detach();
  });

  // tests waiters queue
  for (int i = 0; i < 2; ++i)
    get_bot_info(bot).start_and_detach();
  int i = 0;
retry:
  try {
    for (; i < 2; ++i) {
      start_main_task(bot);
      bot.run();
      TGBM_LOG_INFO("OK!!!");
    }
  } catch (tgbm::timeout_exception& e) {
    TGBM_LOG_ERROR("{}", e.what());
    ++i;
    goto retry;
  } catch (std::exception& e) {
    TGBM_LOG_ERROR("main ended with {}", e.what());
    ++i;
    goto retry;
  }
  return 0;
} catch (...) {
  TGBM_LOG_INFO("ENDED");
}
