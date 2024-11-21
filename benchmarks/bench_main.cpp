#include <benchmark/benchmark.h>

#include <fuzzing.hpp>
#include <types.hpp>

#include <tgbm/api/types/all.hpp>
#include <tgbm/utils/formatters.hpp>

int main(int argc, char** argv) {
  auto& storage = fuzzing::GetMutableStorage();
  // clang-format off
  storage.Add<tgbm::api::User>(
    "SimpleUser", 
    fuzzing::Config{
    .path = "data/user.json",
    .expand_chance = 1.0,
  });

  storage.Add<tgbm::api::Message>(
    "SmallMessage",
    fuzzing::Config{
    .path = "data/message_small.json",
    .max_nesting = 1,
  });

  storage.Add<tgbm::api::Message>(
    "MediumMessage",
    fuzzing::Config{
    .path = "data/message_medium.json",
    .max_nesting = 2,
  });

  storage.Add<tgbm::api::Message>(
    "HardMessage",
    fuzzing::Config{
    .path = "data/message_hard.json",
    .expand_chance = 0.9,
    .nesting_chance = 0.8,
    .max_nesting = 3,
  });

  storage.Add<types::Organization>(
    "Nesting",
    fuzzing::Config{
      .path = "data/nesting.json",
      .nesting_chance = 0.9,
      .max_nesting = 32,
    }
  );

  storage.Add<types::TreeNode>(
    "Tree",
    fuzzing::Config{
      .path = "data/tree.json",
      .expand_chance = 1.0,
      .nesting_chance = 1.0,
      .max_nesting = 16,
    }
  );

  storage.AddArray<types::Update, 1>(
    "Updates_1",
    fuzzing::Config{
      .path = "data/updates_1.json",
      .expand_chance = 0.8,
      .max_nesting = 5,
    }
  );
  storage.AddArray<types::Update, 5>(
    "Updates_5",
    fuzzing::Config{
      .path = "data/updates_5.json",
      .expand_chance = 0.8,
      .max_nesting = 5,
    }
  );
  storage.AddArray<types::Update, 10>(
    "Updates_10",
    fuzzing::Config{
      .path = "data/updates_10.json",
      .expand_chance = 0.8,
      .max_nesting = 5,
    }
  );
  storage.AddArray<types::Update, 50>(
    "Updates_50",
    fuzzing::Config{
      .path = "data/updates_50.json",
      .expand_chance = 0.8,
      .max_nesting = 5,
    }
  );

  storage.AddArray<types::Update, 100>(
    "Updates_100",
    fuzzing::Config{
      .path = "data/updates_100.json",
      .expand_chance = 0.8,
      .max_nesting = 5,
    }
  );

  storage.Add<types::TreeNode>(
    "RawTree",
    fuzzing::Config{
      .path = "data/raw_tree.json",
      .pretty = false,
      .expand_chance = 1.0,
      .nesting_chance = 1.0,
      .max_nesting = 16,
    }
  );

  storage.AddArray<types::Update, 1>(
    "RawUpdates_1",
    fuzzing::Config{
      .path = "data/raw_updates_1.json",
      .pretty = false,
      .expand_chance = 0.8,
      .max_nesting = 5,
    }
  );
  storage.AddArray<types::Update, 5>(
    "RawUpdates_5",
    fuzzing::Config{
      .path = "data/raw_updates_5.json",
      .pretty = false,
      .expand_chance = 0.8,
      .max_nesting = 5,
    }
  );
  storage.AddArray<types::Update, 10>(
    "RawUpdates_10",
    fuzzing::Config{
      .path = "data/raw_updates_10.json",
      .pretty = false,
      .expand_chance = 0.8,
      .max_nesting = 5,
    }
  );
  storage.AddArray<types::Update, 50>(
    "RawUpdates_50",
    fuzzing::Config{
      .path = "data/raw_updates_50.json",
      .pretty = false,
      .expand_chance = 0.8,
      .max_nesting = 5,
    }
  );

  storage.AddArray<types::Update, 100>(
    "RawUpdates_100",
    fuzzing::Config{
      .path = "data/raw_updates_100.json",
      .pretty = false,
      .expand_chance = 0.8,
      .max_nesting = 5,
    }
  );
  // clang-format on

  ::benchmark ::Initialize(&argc, argv);
  if (::benchmark ::ReportUnrecognizedArguments(argc, argv))
    return 1;
  ::benchmark ::RunSpecifiedBenchmarks();
  ::benchmark ::Shutdown();
  return 0;
}
