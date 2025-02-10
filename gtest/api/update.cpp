#include <gtest/gtest.h>

#include <tgbm/api/arrayof.hpp>
#include <tgbm/api/telegram_answer.hpp>
#include <tgbm/json.hpp>
#include <tgbm/api/types/all.hpp>

// https://github.com/bot-motherlib/TGBM/issues/100
TEST(Update, Issues100) {
  std::string_view str = R"({
        "ok": true,
        "result": [
          {
            "update_id": 260770455,
            "my_chat_member": {
              "chat": {
                "id": 109,
                "first_name": "тест",
                "username": "s8",
                "type": "private"
              },
              "from": {
                "id": 109,
                "is_bot": false,
                "first_name": "тест",
                "username": "s8",
                "language_code": "ru"
              },
              "date": 1739094456,
              "old_chat_member": {
                "user": {
                  "id": 115,
                  "is_bot": true,
                  "first_name": "Ebot",
                  "username": "Ebot_bot"
                },
                "status": "kicked",
                "until_date": 0
              },
              "new_chat_member": {
                "user": {
                  "id": 115,
                  "is_bot": true,
                  "first_name": "Ebot",
                  "username": "Ebot_bot"
                },
                "status": "member"
              }
            }
          },
          {
            "update_id": 260770456,
            "message": {
              "message_id": 127765,
              "from": {
                "id": 109,
                "is_bot": false,
                "first_name": "тест",
                "username": "s8",
                "language_code": "ru"
              },
              "chat": {
                "id": 109,
                "first_name": "тест",
                "username": "s8",
                "type": "private"
              },
              "date": 1739094456,
              "text": "/start",
              "entities": [
                {
                  "offset": 0,
                  "length": 6,
                  "type": "bot_command"
                }
              ]
            }
          }
        ]
      }
      )";

  using namespace tgbm::api;
  tgbm::api::arrayof<tgbm::api::Update> got;
  tgbm::api::telegram_answer<tgbm::api::arrayof<tgbm::api::Update>> updates(got);
  tgbm::from_json(str, updates);

  tgbm::api::arrayof<tgbm::api::Update> expected{
      tgbm::api::Update{
          .update_id = 260770455,
          .data =
              Update::my_chat_member{
                  ChatMemberUpdated{
                      .chat =
                          Chat{
                              .id = 109,
                              .type = "private",
                              .username = "s8",
                              .first_name = "тест",
                          },
                      .from =
                          User{
                              .id = 109,
                              .is_bot = false, 
                              .first_name = "тест",
                              .username = "s8",
                              .language_code = "ru",
                          },
                      .date = 1739094456,
                      .old_chat_member =
                          ChatMember{
                              .data =
                                  ChatMemberBanned{
                                      .user =
                                          User{
                                              .id = 115,
                                              .is_bot = true,
                                              .first_name = "Ebot",
                                              .username = "Ebot_bot",
                                          },
                                      .until_date = 0,
                                  },
                          },
                      .new_chat_member =
                          ChatMember{
                              .data =
                                  ChatMemberMember{
                                      .user =
                                          User{
                                              .id = 115,
                                              .is_bot = true,
                                              .first_name = "Ebot",
                                              .username = "Ebot_bot",
                                          },
                                  },
                          },
                  },
              },
      },
      Update{
        .update_id = 260770456,
        .data = Update::message{
            .value = Message
            {
                .message_id = 127765,
                .date = 1739094456,
                .chat = Chat{
                    .id = 109,
                    .type = "private",
                    .username = "s8",
                    .first_name = "тест",
                },
                .from = User{
                    .id = 109,
                    .is_bot = false,
                    .first_name = "тест",
                    .username = "s8",
                    .language_code = "ru"
                },
                .text = "/start",
                .entities = arrayof<MessageEntity>{
                    MessageEntity{
                    .type = "bot_command",
                    .offset = 0,
                    .length = 6,
                    },
                },
            },
        }
      }};

  auto got_json = boost::json::value_from(got);
  auto exp_json = boost::json::value_from(expected);

  EXPECT_EQ(got_json, exp_json);
}
