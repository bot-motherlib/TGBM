#include <gtest/gtest.h>

#include <string>
#include <algorithm>

#include <tgbm/api/methods/all.hpp>
#include <tgbm/json.hpp>
#include <tgbm/api/requests.hpp>

TEST(http, body_generation) {
  tgbm::api::get_updates_request request;
  std::string_view expected_body =
      R"({"offset":12345,"limit":100,"timeout":30,"allowed_updates":["message","edited_channel_post"]})";
  request.offset = 12345;
  request.limit = 100;
  request.timeout = 30;
  request.allowed_updates = {"message", "edited_channel_post"};
  tgbm::http_body req = tgbm::api::make_http_body(request);
  EXPECT_EQ(req.content_type, "application/json");
  std::string generated_body{(const char*)req.data.data(), req.data.size()};
  EXPECT_EQ(generated_body, expected_body);
}

TEST(http, body_generation_hard) {
  tgbm::api::copy_message_request request;
  request.chat_id = "@targetChatUsername";
  request.from_chat_id = -123456789;
  request.message_id = 42;
  request.caption = "example caption";
  request.parse_mode = "Markdown";
  request.disable_notification = true;
  auto& keyboard = request.reply_markup.emplace().emplace<tgbm::api::InlineKeyboardMarkup>();
  keyboard.inline_keyboard.emplace_back()
      .emplace_back(tgbm::api::InlineKeyboardButton{.text = "Button1"})
      .data.emplace<tgbm::api::InlineKeyboardButton::callback_data>()
      .value = "hello world";
  std::string expected_body =
      R"({"chat_id":"@targetChatUsername","from_chat_id":-123456789,"message_id":42,)"
      R"("caption":"example caption","parse_mode":"Markdown","disable_notification":true,)"
      R"("reply_markup":{"inline_keyboard":[[{"text":"Button1","callback_data":"hello world"}]]}})";

  tgbm::http_body req = tgbm::api::make_http_body(request);
  EXPECT_EQ(req.content_type, "application/json");
  std::string generated_body{(const char*)req.data.data(), req.data.size()};
  EXPECT_EQ(generated_body, expected_body);
}
