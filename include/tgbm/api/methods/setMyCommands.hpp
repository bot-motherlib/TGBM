#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/BotCommandScope.hpp"
#include "tgbm/api/types/BotCommand.hpp"

namespace tgbm::api {

struct set_my_commands_request {
  /* A JSON-serialized list of bot commands to be set as the list of the bot's commands. At most 100 commands
   * can be specified. */
  arrayof<BotCommand> commands;
  /* A JSON-serialized object, describing scope of users for which the commands are relevant. Defaults to
   * BotCommandScopeDefault. */
  box<BotCommandScope> scope;
  /* A two-letter ISO 639-1 language code. If empty, commands will be applied to all users from the given
   * scope, for whose language there are no dedicated commands */
  optional<String> language_code;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setMyCommands";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("commands", commands);
    if (scope)
      body.arg("scope", *scope);
    if (language_code)
      body.arg("language_code", *language_code);
  }
};

}  // namespace tgbm::api
