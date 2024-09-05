#if 0

#include <gtest/gtest.h>

#include <tgbm/tools/to_boost_json.h>
#include <tgbm/api/types_all.hpp>
#include <tgbm/tools/from_boost_json.h>
#include <tgbm/tools/json_tools/parser/all.h>
#include <tgbm/tools/random.h>
#include "chaotic_test.hpp"
#include <helper.hpp>
#include <fmt/ranges.h>

namespace api = tgbm::api;

FUZZING_TEST(UserProfiles) {
  tgbm::api::UserProfilePhotos p1, p2;
  tgbm::api::Update expected{.update_id = 12253213,
                             .message = {tgbm::api::Message{.message_id = 5069,
                                                            .from = tgbm::api::User{.id = 1225,
                                                                                    .is_bot = false,
                                                                                    .first_name = "MRV",
                                                                                    .username = "MRV",
                                                                                    .language_code = "ru"},
                                                            .chat = tgbm::api::Chat{.id = 1225,
                                                                                    .first_name = "MRV",
                                                                                    .username = "MRV",
                                                                                    .type = "private"},
                                                            .date = 1724526676,
                                                            .text = "1111"}}};

  auto got = tgbm::from_boost_json<tgbm::api::Update>(R"(
    {
      "update_id": 12253213,
      "message":  {
        "message_id":5069,
        "from":
          { 
            "id":1225,
            "is_bot":false,
            "first_name":"MRV",
            "username":"MRV",
            "language_code":"ru"
          },
        "chat":{
          "id":1225,
          "first_name":"MRV",
          "username":"MRV",
          "type":"private"
        },
        "date":1724526676,
        "text":"1111"
      }
    }
  )");
  EXPECT_EQ(got, expected);
}

//     auto got = tgbm::from_boost_json<api::MenuButton>(j);
//     auto expected = api::MenuButton{
//         .data = api::MenuButtonDefault{}
//     };
//     EXPECT_EQ(got, expected);
// }

#endif
