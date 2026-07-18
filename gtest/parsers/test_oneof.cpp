#include <parsers/fixtures.hpp>

namespace test_oneof {

struct User {
  tgbm::api::Integer id;
  std::string name;

  consteval static bool is_mandatory_field(std::string_view name) {
    return true;
  }

  bool operator==(const User& other) const {
    return id == other.id && name == other.name;
  }
};

struct Chat {
  tgbm::api::Integer id;
  std::string title;

  consteval static bool is_mandatory_field(std::string_view name) {
    return true;
  }

  bool operator==(const Chat& other) const = default;
};

struct maybe {
  tgbm::box<std::string> type;

  consteval static bool is_mandatory_field(std::string_view name) {
    return false;
  }

  bool operator==(const maybe& other) const = default;
};

struct Channel {
  tgbm::api::Integer id;
  std::string name;
  tgbm::box<maybe> maybe;

  consteval static bool is_mandatory_field(std::string_view name) {
    return name == "id" || name == "name";
  }

  bool operator==(const Channel& other) const = default;
};

struct MessageOriginUser {
  tgbm::api::Integer date;
  tgbm::box<User> sender_user;

  consteval static bool is_mandatory_field(std::string_view name) {
    return true;
  }

  bool operator==(const MessageOriginUser& other) const = default;
};

struct MessageOriginHiddenUser {
  tgbm::api::Integer date;
  std::string sender_name;

  consteval static bool is_mandatory_field(std::string_view name) {
    return true;
  }

  bool operator==(const MessageOriginHiddenUser& other) const = default;
};

struct MessageOriginChat {
  tgbm::api::Integer date;
  tgbm::box<Chat> sender_chat;

  consteval static bool is_mandatory_field(std::string_view name) {
    return true;
  }

  bool operator==(const MessageOriginChat& other) const = default;
};

struct MessageOriginChannel {
  tgbm::api::Integer date;
  tgbm::box<Channel> sender_channel;

  consteval static bool is_mandatory_field(std::string_view name) {
    return name == "date" || name == "sender_channel";
  }

  bool operator==(const MessageOriginChannel& other) const = default;
};

using MessageOriginData =
    tgbm::api::oneof<MessageOriginUser, MessageOriginHiddenUser, MessageOriginChat, MessageOriginChannel>;

struct MessageOrigin {
  MessageOriginData data;

  bool operator==(const MessageOrigin& other) const {
    return data == other.data;
  }

  static constexpr std::string_view discriminator = "type";

  enum struct type_e { k_user, k_hidden_user, k_chat, k_channel, nothing };

  static constexpr auto variant_size = size_t(type_e::nothing);

  static constexpr type_e discriminate(std::string_view val) {
    return tgbm::string_switch<type_e>(val)
        .case_("user", type_e::k_user)
        .case_("hidden_user", type_e::k_hidden_user)
        .case_("chat", type_e::k_chat)
        .case_("channel", type_e::k_channel)
        .or_default(type_e::nothing);
  }
  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "user")
      return visitor.template operator()<MessageOriginUser>();
    if (val == "hidden_user")
      return visitor.template operator()<MessageOriginHiddenUser>();
    if (val == "chat")
      return visitor.template operator()<MessageOriginChat>();
    if (val == "channel")
      return visitor.template operator()<MessageOriginChannel>();
    return visitor.template operator()<void>();
  }
  type_e type() const {
    return static_cast<type_e>(data.index());
  }

  std::string_view discriminator_now() const noexcept {
    switch (type()) {
      case MessageOrigin::type_e::k_user:
        return "user";
      case MessageOrigin::type_e::k_hidden_user:
        return "hidden_user";
      case MessageOrigin::type_e::k_chat:
        return "chat";
      case MessageOrigin::type_e::k_channel:
        return "channel";
      case MessageOrigin::type_e::nothing:
        return "nothing";
    }
    tgbm::unreachable();
  }
};

JSON_PARSE_TEST(OneOfUserType, MessageOrigin) {
  MessageOrigin expected{
      .data =
          MessageOriginUser{
              .date = tgbm::api::Integer{1630454400},
              .sender_user =
                  tgbm::box<User>{
                      User{
                          .id = tgbm::api::Integer{12345},
                          .name = "John Doe",
                      },
                  },
          },
  };

  auto json = R"(
    {
      "type": "user",
      "date": 1630454400,
      "sender_user": {
        "id": 12345,
        "name": "John Doe"
      }
    }
  )";

  MessageOrigin parsed = parse_json(json);
  EXPECT_EQ(parsed, expected);
}

JSON_PARSE_TEST(OneOfHiddenUserType, MessageOrigin) {
  MessageOrigin expected{
      .data =
          MessageOriginHiddenUser{
              .date = tgbm::api::Integer{1630454400},
              .sender_name = "Anonymous User",
          },
  };

  auto json = R"(
    {
      "type": "hidden_user",
      "date": 1630454400,
      "sender_name": "Anonymous User"
    }
  )";

  MessageOrigin parsed = parse_json(json);
  EXPECT_EQ(parsed, expected);
}

JSON_PARSE_TEST(OneOfChatType, MessageOrigin) {
  MessageOrigin expected{
      .data =
          MessageOriginChat{
              .date = tgbm::api::Integer{1630454400},
              .sender_chat =
                  tgbm::box<Chat>{
                      Chat{
                          .id = tgbm::api::Integer{67890},
                          .title = "Group Chat",
                      },
                  },
          },
  };

  auto json = R"(
    {
      "type": "chat",
      "date": 1630454400,
      "sender_chat": {
        "id": 67890,
        "title": "Group Chat"
      }
    }
  )";

  MessageOrigin parsed = parse_json(json);
  EXPECT_EQ(parsed, expected);
}

JSON_PARSE_TEST(OneOfChannelType, MessageOrigin) {
  MessageOrigin expected{
      .data =
          MessageOriginChannel{
              .date = tgbm::api::Integer{1630454400},
              .sender_channel =
                  tgbm::box<Channel>{
                      Channel{
                          .id = tgbm::api::Integer{13579},
                          .name = "News Channel",
                      },
                  },
          },
  };

  auto json = R"(
    {
      "type": "channel",
      "date": 1630454400,
      "sender_channel": {
        "id": 13579,
        "name": "News Channel"
      }
    }
  )";

  MessageOrigin parsed = parse_json(json);
  EXPECT_EQ(parsed, expected);
}

JSON_PARSE_TEST(OneOfUnknownType, MessageOrigin) {
  MessageOrigin expected;
  auto json = R"(
        {
        "type": "unknown_type",
        "date": 1630454400
        }
    )";
  MessageOrigin parsed = parse_json(json);
  EXPECT_EQ(parsed, expected);
}

JSON_PARSE_TEST(OneOfHiddenUserTypeMoreData, MessageOrigin) {
  MessageOrigin expected{
      .data =
          MessageOriginHiddenUser{
              .date = tgbm::api::Integer{1630454400},
              .sender_name = "Anonymous User",
          },
  };
  auto json = R"(
        {
          "type": "hidden_user",
          "date": 1630454400,
          "sender_name": "Anonymous User",
          "more_data": "some_info"
        }
    )";

  MessageOrigin parsed = parse_json(json);
  EXPECT_EQ(parsed, expected);
}

TEST(GeneratorBoost, MissplaceDiscriminator) {
  MessageOrigin expected{
      .data =
          MessageOriginChannel{
              .date = tgbm::api::Integer{1630454400},
              .sender_channel =
                  tgbm::box<Channel>{
                      Channel{
                          .id = tgbm::api::Integer{13579},
                          .name = "News Channel",
                      },
                  },
          },
  };

  auto json = R"(
    {
      "date": 1630454400,
      "sender_channel": {
        "id": 13579,
        "name": "News Channel"
      },
      "type": "channel"
    }
  )";

  MessageOrigin parsed = GeneratorBoost::parse_json<MessageOrigin>(json);
  EXPECT_EQ(parsed, expected);
}

TEST(GeneratorBoost, ArrayMissplaceDiscriminator) {
  std::vector<MessageOrigin> expected{
      MessageOrigin{
          .data =
              MessageOriginChannel{
                  .date = tgbm::api::Integer{1630454400},
                  .sender_channel =
                      tgbm::box<Channel>{
                          Channel{
                              .id = tgbm::api::Integer{13579},
                              .name = "News Channel",
                          },
                      },
              },
      },
      MessageOrigin{
          .data =
              MessageOriginChat{
                  .date = tgbm::api::Integer{1630454400},
                  .sender_chat =
                      tgbm::box<Chat>{
                          Chat{
                              .id = tgbm::api::Integer{67890},
                              .title = "Group Chat",
                          },
                      },
              },
      },
  };

  auto json = R"(
[
   {
      "date":1630454400,
      "type":"channel",
      "sender_channel":{
         "id":13579,
         "name":"News Channel"
      }
   },
   {
      "date":1630454400,
      "sender_chat":{
         "id":67890,
         "title":"Group Chat"
      },
      "type":"chat"
   }
])";

  auto parsed = GeneratorBoost::parse_json<std::vector<MessageOrigin>>(json);
  EXPECT_EQ(parsed, expected);
}

JSON_PARSE_TEST(MissingField, MessageOrigin) {
  MessageOrigin expected;
  auto json = R"(
{
  "date": 1630454400
}
    )";
  EXPECT_THROW(parse_json(json), tgbm::json::parse_error);
}

TEST(fails, injson) {
  // invalid discriminator, ignoring value (API back compatibility, may be new type added)
  auto json = R"(
[
   {
      "date":1630454400,
      "type":"chanel",
      "sender_channel":{
         "id":13579,
         "name":"News Channel"
      }
   },
   {
      "date":1630454400,
      "sender_chat":{
         "id":67890,
         "title":"Group Chat"
      },
      "type":"chat"
   }
])";
  std::vector<MessageOrigin> res;
  tgbm::json::stream_parser parser(res);
  EXPECT_NO_THROW(parser.feed(json, /*end=*/true));
}

TEST(fails2, injson) {
  // no required field `sender_channel`
  auto json = R"(
[
   {
      "date":1630454400,
      "type":"channel"
   }
])";
  std::vector<MessageOrigin> res;
  tgbm::json::stream_parser parser(res);
  try {
    parser.feed(json, /*end=*/true);
    FAIL();
  } catch (tgbm::json::parse_error& e) {
    EXPECT_TRUE(std::string(e.what()).find(json) != std::string::npos);
    EXPECT_TRUE(std::string(e.what()).find("not all required fields are present") != std::string::npos);
  }
}

TEST(fails3, injson) {
  // syntax error (additional `,` at end of object)
  auto json = R"(
[
   {
      "date":1630454400,
      "sender_channel":{
         "id":13579,
         "name":"News Channel"
      },
      "type":"channel",
   }
])";
  std::vector<MessageOrigin> res;
  tgbm::json::stream_parser parser(res);
  try {
    parser.feed(json, /*end=*/true);
    FAIL();
  } catch (tgbm::json::parse_error& e) {
    EXPECT_TRUE(std::string(e.what()).find(json) != std::string::npos);
    EXPECT_TRUE(std::string(e.what()).find("syntax error") != std::string::npos);
  }
}

TEST(discriminated_oneof, oneof) {
  // проверяет что дискриминатор "type" лежащий ближе по токенам чем настоящий дискриминатор игнорируется
  std::string_view json = R"(
   {
      "date":1630454400,
      "sender_channel":{
         "id":13579,
         "name":"News Channel",
         "maybe": {
          "type": "invalid_discriminator"
         }
      },
      "type":"channel"
   })";
  {
    MessageOrigin res;
    tgbm::json::stream_parser parser(res);
    parser.feed(json, /*end=*/true);
    auto* m = res.data.get_if<MessageOriginChannel>();
    EXPECT_TRUE(m && m->sender_channel->maybe && m->sender_channel->maybe->type == "invalid_discriminator");
  }
  // parts
  {
    MessageOrigin res;
    tgbm::json::stream_parser parser(res);
    std::string_view j = json;
    while (!j.empty()) {
      parser.feed(j.substr(0, 1), /*end=*/j.size() == 1);
      j = j.substr(1);
    }
    auto* m = res.data.get_if<MessageOriginChannel>();
    EXPECT_TRUE(m && m->sender_channel->maybe && m->sender_channel->maybe->type == "invalid_discriminator");
  }
}

}  // namespace test_oneof
