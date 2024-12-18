TGBM is a easy-to-use, flexible C++20 library for creating telegram bots

[![build and test](
https://github.com/bot-motherlib/TGBM/actions/workflows/CI_ubuntu.yaml/badge.svg?branch=master)](
https://github.com/bot-motherlib/TGBM/actions/workflows/CI_ubuntu.yaml)

### Why Choose TGBM?

- **Easy Integration**: by CPM or even cmake fetch content
- **High Performance**: designed with efficiency and speed in mind, ensuring optimal performance.  
- **User-Friendly**: simple, intuitive, developer-friendly API for a smooth experience.  
- **Extensible**: flexible and modular; does not impose unnecessary abstractions or dependencies, leaving room for your own design.
- **HTTP2**

### Adding with CMake:

Preferred way with [CPM](https://github.com/cpm-cmake/CPM.cmake)

```cmake

CPMAddPackage(
  NAME TGBM
  GIT_REPOSITORY https://github.com/bot-motherlib/TGBM
  GIT_TAG        origin/master
  OPTIONS "TGBM_ENABLE_EXAMPLES ON"
)

target_link_libraries(MyTargetName tgbmlib)

```

simple way with fetch content:

```cmake

include(FetchContent)
FetchContent_Declare(
  TGBM
  GIT_REPOSITORY https://github.com/bot-motherlib/TGBM
  GIT_TAG        origin/master
)
FetchContent_MakeAvailable(TGBM)
target_link_libraries(MyTargetName tgbmlib)

```

Then just run

```bash
cmake .
```
Note: its recomended for windows to use clang and Ninja (cmake -G Ninja)

## API overview

### echobot

This bot responds with the same text that the user sent it

[examples/echobot](https://github.com/bot-motherlib/TGBM/tree/master/examples/echobot)


```cpp

#include <tgbm/bot.hpp>

dd::task<void> coro_main(tgbm::bot& bot) {
  // bot presented as sequence of updates

  co_foreach(tgbm::api::Update u, bot.updates()) {
    tgbm::api::Message* m = u.get_message();

    if (!m || !m->text)
      continue;

    co_await bot.api.sendMessage({
        .chat_id = m->chat->id,
        .text = *m->text,
    });
  }
}

int main() {
  tgbm::bot bot{/* your bot token from @BotFather */};

  coro_main(bot).start_and_detach();
  bot.run();

  return 0;
}

```
<div align="center">
  <img src="https://github.com/user-attachments/assets/e3676389-e587-475c-bb25-6c6fd141a2ca" alt="image" width="600px">
</div>

