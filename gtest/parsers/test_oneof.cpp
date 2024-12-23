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

struct Channel {
  tgbm::api::Integer id;
  std::string name;

  consteval static bool is_mandatory_field(std::string_view name) {
    return true;
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
    return true;
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
  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visiter) {
    if (val == "user")
      return visiter.template operator()<MessageOriginUser>();
    if (val == "hidden_user")
      return visiter.template operator()<MessageOriginHiddenUser>();
    if (val == "chat")
      return visiter.template operator()<MessageOriginChat>();
    if (val == "channel")
      return visiter.template operator()<MessageOriginChannel>();
    return visiter.template operator()<void>();
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

}  // namespace test_oneof
