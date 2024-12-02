#if 0

  #include <cstdlib>
  #include <filesystem>
  #include <string>
  #include <fstream>

  #include <tgbm/bot.hpp>
  #include <tgbm/logger.hpp>
  #include <tgbm/net/http2/client.hpp>
  #include <tgbm/net/http2/server.hpp>

struct print_server : tgbm::http2_server {
  using tgbm::http2_server::http2_server;

  dd::task<tgbm::http_response> handle_request(tgbm::http_request req) {
    tgbm::http_response& rsp = co_await dd::this_coro::return_place;
    rsp.status = 200;
    std::string answer = R"(
{
  "ok": true,
  "result": {
    "id": 123456789,
    "is_bot": true,
    "first_name": "MyBot",
    "username": "MyBotUsername",
    "can_join_groups": true,
    "can_read_all_group_messages": false,
    "supports_inline_queries": false
  }
}
)";
    rsp.body.insert(rsp.body.end(), answer.begin(), answer.end());
    fmt::println("request, path: {}, body {}", req.path,
                 std::string_view((const char*)req.body.data.data(), req.body.data.size()));
    co_return dd::rvo;
  }
};

// ########################################## NOT SERVER ################################

dd::task<void> get_bot_info(tgbm::bot& b) {
  auto info = co_await b.api.getMe();
  TGBM_LOG("user: {} ({})", info.first_name, info.username.value_or("<no-username>"));
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
  auto msg = co_await bot.api.sendAudio(
      {.chat_id = tgbm::api::int_or_str(chatid),
       .audio = tgbm::api::file_or_str(tgbm::api::InputFile::from_file(p.string(), "audio/ogg"))});
  std::filesystem::remove(p);
}

dd::task<void> main_coro(tgbm::bot& bot) {
  co_foreach(tgbm::api::Update && u, bot.updates({.drop_pending_updates = true})) {
    tgbm::api::Message* message = u.get_message();
    if (!message)
      continue;
    if (message->text)
      fmt::println("user wrote {}", *message->text);
    if (message->voice)
      download_audio(bot, message->chat->id, message->voice->file_id).start_and_detach();
    else if (message->text)
      bot.api.sendMessage({.chat_id = message->chat->id, .text = "your message is: " + *message->text})
          .start_and_detach();
  }
}

int main() try {
  const char* token = std::getenv("BOT_TOKEN");
  if (!token) {
    fmt::println("launching telegram bot requires bot token from @BotFather");
    return -1;
  }
  tgbm::tcp_connection_options opts{
      .disable_ssl_certificate_verify = true,
  };
  tgbm::bot bot(token, std::unique_ptr<tgbm::http_client>(new tgbm::http2_client("127.0.0.1", {}, opts)));

  bot.commands.add("stop", [&bot](tgbm::api::Message&&) { bot.stop(); });
  bot.commands.add("send_file", [&bot](tgbm::api::Message&& message) {
    tgbm::api::InputFile p = tgbm::api::InputFile::from_file("E:/test_transmission_file.txt", "text/plain");
    bot.api.sendDocument({.chat_id = message.chat->id, .document = std::move(p)}).start_and_detach();
  });
  bot.commands.add("send_big_file", [&bot](tgbm::api::Message&& message) {
    tgbm::api::InputFile p = tgbm::api::InputFile::from_file("E:/test_file_5MB.txt", "text/plain");
    bot.api.sendDocument({.chat_id = message.chat->id, .document = std::move(p)}).start_and_detach();
  });

  // ################# SERVER

  tgbm::http2_server_ptr server = new print_server(tgbm::http2_server_options{
      .ssl_cert_path = "E:/dev/ssl_test_crt/server.crt",
      .private_key_path = "E:/dev/ssl_test_crt/server.key",
  });
  namespace asio = boost::asio;
  asio::ip::tcp::endpoint ipv4_endpoint(asio::ip::address_v4::loopback(), 443);
  server->listen(ipv4_endpoint);

  asio::ip::tcp::endpoint ipv6_endpoint(asio::ip::address_v6::loopback(), 443);
  server->listen(ipv6_endpoint);

  server->start();

  // ################## NOT SERVER
  // tests waiters queue
  for (int i = 0; i < 2; ++i)
    get_bot_info(bot).start_and_detach();

  int i = 0;
retry:
  try {
    for (; i < 2; ++i) {
        main_coro(bot).start_and_detach();
      bot.run();
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
  fmt::println("ENDED");
}

#endif

#include <cstdlib>
#include <filesystem>
#include <string>
#include <fstream>

#include <tgbm/bot.hpp>
#include <tgbm/logger.hpp>
#include <tgbm/net/http2/client.hpp>
#include <tgbm/utils/scope_exit.hpp>

dd::task<void> get_bot_info(tgbm::bot& b) {
  auto info = co_await b.api.getMe();
  TGBM_LOG("user: {} ({})", info.first_name, info.username.value_or("<no-username>"));
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
  auto msg = co_await bot.api.sendAudio(
      {.chat_id = tgbm::api::int_or_str(chatid),
       .audio = tgbm::api::file_or_str(tgbm::api::InputFile::from_file(p.string(), "audio/ogg"))});
  std::filesystem::remove(p);
}

dd::task<void> main_coro(tgbm::bot& bot) {
  on_scope_exit {
    bot.stop();
  };
  co_foreach(tgbm::api::Update && u, bot.updates({.drop_pending_updates = true})) {
    tgbm::api::Message* message = u.get_message();
    if (!message)
      continue;
    if (message->text)
      fmt::println("user wrote {}", *message->text);
    if (message->voice)
      download_audio(bot, message->chat->id, message->voice->file_id).start_and_detach();
    else if (message->text)
      bot.api.sendMessage({.chat_id = message->chat->id, .text = "your message is: " + *message->text})
          .start_and_detach();
  }
}

int main() try {
  const char* token = std::getenv("BOT_TOKEN");
  if (!token) {
    fmt::println("launching telegram bot requires bot token from @BotFather");
    return -1;
  }
  tgbm::bot bot(token, std::unique_ptr<tgbm::http_client>(new tgbm::http2_client));

  bot.commands.add("stop", [&bot](tgbm::api::Message&&) { bot.stop(); });
  bot.commands.add("send_file", [&bot](tgbm::api::Message&& message) {
    tgbm::api::InputFile p = tgbm::api::InputFile::from_file("E:/test_transmission_file.txt", "text/plain");
    bot.api.sendDocument({.chat_id = message.chat->id, .document = std::move(p)}).start_and_detach();
  });
  bot.commands.add("send_big_file", [&bot](tgbm::api::Message&& message) {
    tgbm::api::InputFile p = tgbm::api::InputFile::from_file("E:/test_file_5MB.txt", "text/plain");
    bot.api.sendDocument({.chat_id = message.chat->id, .document = std::move(p)}).start_and_detach();
  });

  // tests waiters queue
  for (int i = 0; i < 2; ++i)
    get_bot_info(bot).start_and_detach();

  int i = 0;
retry:
  try {
    for (; i < 2; ++i) {
      main_coro(bot).start_and_detach();
      bot.run();
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
  fmt::println("ENDED");
}
