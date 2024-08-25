#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/input/KeyMappings.h"
#include "quoll/yaml/Yaml.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheTestBase.h"
#include <GLFW/glfw3.h>
#include <random>

static const quoll::Path FilePath =
    AssetCacheTestBase::CachePath / "test.inputmap";

class AssetCacheInputMapTest : public AssetCacheTestBase {
public:
  quoll::Result<quoll::AssetHandle<quoll::InputMapAsset>>
  loadInputMap(std::function<void(YAML::Node &)> fn,
               quoll::Uuid uuid = quoll::Uuid::generate()) {
    auto filePath = cache.getPathFromUuid(uuid);

    // prepopulate with valid values
    YAML::Node node;
    node["version"] = "0.1";
    node["type"] = "inputmap";

    {
      YAML::Node item;
      item["name"] = "Test scheme";

      node["schemes"].push_back(item);
    }

    {
      YAML::Node item;
      item["name"] = "Test command";
      item["type"] = "boolean";

      node["commands"].push_back(item);
    }

    {
      YAML::Node item;
      item["scheme"] = 0;
      item["command"] = 0;
      item["binding"] = YAML::Null;

      node["bindings"].push_back(item);
    }

    fn(node);

    std::ofstream stream(filePath);
    stream << node;
    stream.close();

    return cache.loadInputMap(uuid);
  };
};

TEST_F(AssetCacheInputMapTest, CreateInputMapFromSource) {
  auto uuid = quoll::Uuid::generate();
  auto filePath =
      cache.createInputMapFromSource(FixturesPath / "test.inputmap", uuid);
  EXPECT_TRUE(filePath.hasData());
  EXPECT_FALSE(filePath.hasError());
  EXPECT_FALSE(filePath.hasWarnings());

  EXPECT_EQ(filePath.getData().filename().string().size(), 38);

  auto meta = cache.getAssetMeta(uuid);

  EXPECT_EQ(meta.type, quoll::AssetType::InputMap);
  EXPECT_EQ(meta.name, "test.inputmap");
}

TEST_F(AssetCacheInputMapTest,
       LoadInputMapFailsIfProvidedPropertiesAreInvalid) {
  {
    auto res = loadInputMap([](auto &node) { node["version"] = "0.2"; });
    EXPECT_TRUE(res.hasError());
  }

  {
    auto res = loadInputMap([](auto &node) { node["type"] = "not-input-map"; });
    EXPECT_TRUE(res.hasError());
  }

  // Control schemes
  {
    auto res = loadInputMap([](auto &node) {
      YAML::Node item;
      item["name"] = "Test scheme";

      node["schemes"].push_back(item);
    });
    EXPECT_TRUE(res.hasError());
  }

  {
    auto res = loadInputMap([](auto &node) { node["schemes"] = "test"; });
    EXPECT_TRUE(res.hasError());
  }

  {
    auto res =
        loadInputMap([](auto &node) { node["schemes"].push_back("test"); });
    EXPECT_TRUE(res.hasError());
  }

  {
    auto res = loadInputMap([](auto &node) {
      YAML::Node item;
      item["name"] = YAML::Node(YAML::NodeType::Sequence);
      node["schemes"].push_back(item);
    });

    EXPECT_TRUE(res.hasError());
  }

  // Commands
  {
    auto res = loadInputMap([](auto &node) {
      YAML::Node item;
      item["name"] = "Test command";
      item["type"] = "boolean";

      node["commands"].push_back(item);
    });
    EXPECT_TRUE(res.hasError());
  }

  {
    auto res = loadInputMap([](auto &node) { node["commands"] = "test"; });
    EXPECT_TRUE(res.hasError());
  }

  {
    auto res =
        loadInputMap([](auto &node) { node["commands"].push_back("test"); });
    EXPECT_TRUE(res.hasError());
  }

  {
    auto res = loadInputMap([](auto &node) {
      YAML::Node item;
      item["name"] = YAML::Node(YAML::NodeType::Sequence);
      item["type"] = "boolean";
      node["commands"].push_back(item);
    });

    EXPECT_TRUE(res.hasError());
  }

  {
    auto res = loadInputMap([](auto &node) {
      YAML::Node item;
      item["name"] = "hello";
      item["type"] = "random-type-name";
      node["commands"].push_back(item);
    });

    EXPECT_TRUE(res.hasError());
  }

  {
    auto res = loadInputMap([](auto &node) {
      YAML::Node item;
      item["name"] = "hello";
      item["type"].push_back("test");
      node["commands"].push_back(item);
    });

    EXPECT_TRUE(res.hasError());
  }

  // Bindings
  {
    auto res = loadInputMap([](auto &node) { node["bindings"] = "test"; });
    EXPECT_TRUE(res.hasError());
  }

  {
    auto res =
        loadInputMap([](auto &node) { node["bindings"].push_back("test"); });
    EXPECT_TRUE(res.hasError());
  }

  {
    auto res = loadInputMap([](auto &node) {
      YAML::Node item;
      item["scheme"] = "test";
      item["command"] = 0;
      item["binding"] = YAML::Null;

      node["bindings"].push_back(item);
    });
    EXPECT_TRUE(res.hasError());
  }

  {
    auto res = loadInputMap([](auto &node) {
      YAML::Node item;
      item["scheme"] = 0;
      item["command"] = "test";
      item["binding"] = YAML::Null;

      node["bindings"].push_back(item);
    });
    EXPECT_TRUE(res.hasError());
  }

  {
    auto res = loadInputMap([](auto &node) {
      YAML::Node item;
      item["scheme"] = 1;
      item["command"] = 0;
      item["binding"] = YAML::Null;

      node["bindings"].push_back(item);
    });
    EXPECT_TRUE(res.hasError());
  }

  {
    auto res = loadInputMap([](auto &node) {
      YAML::Node item;
      item["scheme"] = 0;
      item["command"] = 1;
      item["binding"] = YAML::Null;

      node["bindings"].push_back(item);
    });
    EXPECT_TRUE(res.hasError());
  }
}

TEST_F(AssetCacheInputMapTest,
       LoadInputMapFailsIfProvidedBindingValueIsInvalid) {
  auto res = loadInputMap([](auto &node) {
    node["commands"] = YAML::Node(YAML::NodeType::Sequence);
    node["bindings"] = YAML::Node(YAML::NodeType::Sequence);

    YAML::Node command;
    command["name"] = "Test";
    command["type"] = "boolean";
    node["commands"].push_back(command);

    YAML::Node binding;
    binding["scheme"] = 0;
    binding["command"] = 0;
    binding["binding"] = "RandomText";
    node["bindings"].push_back(binding);
  });
  EXPECT_TRUE(res.hasError());
}

// Boolean
TEST_F(AssetCacheInputMapTest,
       LoadInputMapFailsIfNonBooleanValueIsPassedToBooleanCommandType) {
  auto res = loadInputMap([](auto &node) {
    node["commands"] = YAML::Node(YAML::NodeType::Sequence);
    node["bindings"] = YAML::Node(YAML::NodeType::Sequence);

    YAML::Node command;
    command["name"] = "Test";
    command["type"] = "boolean";
    node["commands"].push_back(command);

    YAML::Node binding;
    binding["scheme"] = 0;
    binding["command"] = 0;
    binding["binding"].push_back("KEY_A");
    binding["binding"].push_back("KEY_B");
    binding["binding"].push_back("KEY_C");
    binding["binding"].push_back("KEY_D");

    node["bindings"].push_back(binding);
  });
  EXPECT_TRUE(res.hasError());
}

TEST_F(AssetCacheInputMapTest,
       LoadInputMapSupportsPassingBooleanValueForBooleanCommandType) {
  auto res = loadInputMap([](auto &node) {
    node["commands"] = YAML::Node(YAML::NodeType::Sequence);
    node["bindings"] = YAML::Node(YAML::NodeType::Sequence);

    YAML::Node command;
    command["name"] = "Test";
    command["type"] = "boolean";
    node["commands"].push_back(command);

    YAML::Node binding;
    binding["scheme"] = 0;
    binding["command"] = 0;
    binding["binding"] = "KEY_A";

    node["bindings"].push_back(binding);
  });

  EXPECT_TRUE(res.hasData());
}

TEST_F(AssetCacheInputMapTest,
       LoadInputMapSupportsPassingNullValueForBooleanCommandType) {
  auto res = loadInputMap([](auto &node) {
    node["commands"] = YAML::Node(YAML::NodeType::Sequence);
    node["bindings"] = YAML::Node(YAML::NodeType::Sequence);

    YAML::Node command;
    command["name"] = "Test";
    command["type"] = "boolean";
    node["commands"].push_back(command);

    YAML::Node binding;
    binding["scheme"] = 0;
    binding["command"] = 0;
    binding["binding"] = YAML::Null;

    node["bindings"].push_back(binding);
  });

  EXPECT_TRUE(res.hasData());
}

// Axis2d
TEST_F(AssetCacheInputMapTest,
       LoadInputMapFailsIfNonAxis2dValueIsPassedToAxis2d) {
  auto res = loadInputMap([](auto &node) {
    node["commands"] = YAML::Node(YAML::NodeType::Sequence);
    node["bindings"] = YAML::Node(YAML::NodeType::Sequence);

    YAML::Node command;
    command["name"] = "Test";
    command["type"] = "axis-2d";
    node["commands"].push_back(command);

    YAML::Node binding;
    binding["scheme"] = 0;
    binding["command"] = 0;
    binding["binding"] = "GAMEPAD_LEFT_X";
    node["bindings"].push_back(binding);
  });

  EXPECT_TRUE(res.hasError());
}

TEST_F(AssetCacheInputMapTest, LoadInputMapSupportsPassingAxis2dValueToAxis2d) {
  auto res = loadInputMap([](auto &node) {
    node["commands"] = YAML::Node(YAML::NodeType::Sequence);
    node["bindings"] = YAML::Node(YAML::NodeType::Sequence);

    YAML::Node command;
    command["name"] = "Test";
    command["type"] = "axis-2d";
    node["commands"].push_back(command);

    YAML::Node binding;
    binding["scheme"] = 0;
    binding["command"] = 0;
    binding["binding"] = "MOUSE_MOVE";
    node["bindings"].push_back(binding);
  });

  EXPECT_TRUE(res.hasData());
}

TEST_F(AssetCacheInputMapTest,
       LoadInputMapFailsIfBothAxesOfExpandedAxis2dAreNotProvided) {
  {
    auto res = loadInputMap([](auto &node) {
      node["commands"] = YAML::Node(YAML::NodeType::Sequence);
      node["bindings"] = YAML::Node(YAML::NodeType::Sequence);

      YAML::Node command;
      command["name"] = "Test";
      command["type"] = "axis-2d";
      node["commands"].push_back(command);

      YAML::Node binding;
      binding["scheme"] = 0;
      binding["command"] = 0;
      binding["binding"]["y"] = "KEY_A";
      node["bindings"].push_back(binding);
    });

    EXPECT_TRUE(res.hasError());
  }

  {
    auto res = loadInputMap([](auto &node) {
      node["commands"] = YAML::Node(YAML::NodeType::Sequence);
      node["bindings"] = YAML::Node(YAML::NodeType::Sequence);

      YAML::Node command;
      command["name"] = "Test";
      command["type"] = "axis-2d";
      node["commands"].push_back(command);

      YAML::Node binding;
      binding["scheme"] = 0;
      binding["command"] = 0;
      binding["binding"]["x"] = "KEY_A";
      node["bindings"].push_back(binding);
    });

    EXPECT_TRUE(res.hasError());
  }
}

TEST_F(AssetCacheInputMapTest,
       LoadInputMapFailsIfNonAxis1DValueIsPassedToExpandedAxis2d) {
  auto res = loadInputMap([](auto &node) {
    node["commands"] = YAML::Node(YAML::NodeType::Sequence);
    node["bindings"] = YAML::Node(YAML::NodeType::Sequence);

    YAML::Node command;
    command["name"] = "Test";
    command["type"] = "axis-2d";
    node["commands"].push_back(command);

    YAML::Node binding;
    binding["scheme"] = 0;
    binding["command"] = 0;
    binding["binding"]["x"] = "KEY_A";
    binding["binding"]["y"] = YAML::Null;
    node["bindings"].push_back(binding);
  });

  EXPECT_TRUE(res.hasError());
}

TEST_F(AssetCacheInputMapTest,
       LoadInputMapSupportsPassingAxis1dValueToExpandedAxis2d) {
  auto res = loadInputMap([](auto &node) {
    node["commands"] = YAML::Node(YAML::NodeType::Sequence);
    node["bindings"] = YAML::Node(YAML::NodeType::Sequence);

    YAML::Node command;
    command["name"] = "Test";
    command["type"] = "axis-2d";
    node["commands"].push_back(command);

    YAML::Node binding;
    binding["scheme"] = 0;
    binding["command"] = 0;
    binding["binding"]["x"] = "GAMEPAD_LEFT_X";
    binding["binding"]["y"] = "GAMEPAD_LEFT_Y";
    node["bindings"].push_back(binding);
  });

  EXPECT_TRUE(res.hasData());
}

TEST_F(
    AssetCacheInputMapTest,
    LoadInputMapFailsIfTwoSegmentsAreNotProvidedToAxisSegmentOfExpandedAxis2d) {
  {
    auto res = loadInputMap([](auto &node) {
      node["commands"] = YAML::Node(YAML::NodeType::Sequence);
      node["bindings"] = YAML::Node(YAML::NodeType::Sequence);

      YAML::Node command;
      command["name"] = "Test";
      command["type"] = "axis-2d";
      node["commands"].push_back(command);

      YAML::Node binding;
      binding["scheme"] = 0;
      binding["command"] = 0;
      binding["binding"]["x"].push_back("KEY_A");
      binding["binidng"]["y"] = YAML::Null;
      node["bindings"].push_back(binding);
    });

    EXPECT_TRUE(res.hasError());
  }

  {
    auto res = loadInputMap([](auto &node) {
      node["commands"] = YAML::Node(YAML::NodeType::Sequence);
      node["bindings"] = YAML::Node(YAML::NodeType::Sequence);

      YAML::Node command;
      command["name"] = "Test";
      command["type"] = "axis-2d";
      node["commands"].push_back(command);

      YAML::Node binding;
      binding["scheme"] = 0;
      binding["command"] = 0;
      binding["binding"]["x"].push_back("KEY_A");
      binding["binding"]["x"].push_back("KEY_B");
      binding["binding"]["x"].push_back("KEY_C");

      binding["binidng"]["y"] = YAML::Null;
      node["bindings"].push_back(binding);
    });

    EXPECT_TRUE(res.hasError());
  }
}

TEST_F(
    AssetCacheInputMapTest,
    LoadInputMapFailsIfNonBooleanValueIsPassedToAxisSegmentOfExpandedOfAxis2d) {
  auto res = loadInputMap([](auto &node) {
    node["commands"] = YAML::Node(YAML::NodeType::Sequence);
    node["bindings"] = YAML::Node(YAML::NodeType::Sequence);

    YAML::Node command;
    command["name"] = "Test";
    command["type"] = "axis-2d";
    node["commands"].push_back(command);

    YAML::Node binding;
    binding["scheme"] = 0;
    binding["command"] = 0;
    binding["binding"]["x"].push_back("GAMEPAD_LEFT_X");
    binding["binding"]["x"].push_back("KEY_D");
    binding["binidng"]["y"] = YAML::Null;
    node["bindings"].push_back(binding);
  });

  EXPECT_TRUE(res.hasError());
}

TEST_F(AssetCacheInputMapTest,
       LoadInputMapSupportsPassingBooleanValueToAxisSegmentOfExpandedAxis2d) {
  auto res = loadInputMap([](auto &node) {
    node["commands"] = YAML::Node(YAML::NodeType::Sequence);
    node["bindings"] = YAML::Node(YAML::NodeType::Sequence);

    YAML::Node command;
    command["name"] = "Test";
    command["type"] = "axis-2d";
    node["commands"].push_back(command);

    YAML::Node binding;
    binding["scheme"] = 0;
    binding["command"] = 0;
    binding["binding"]["x"].push_back("KEY_A");
    binding["binding"]["x"].push_back(YAML::Null);
    binding["binidng"]["y"] = YAML::Null;
    node["bindings"].push_back(binding);
  });

  EXPECT_TRUE(res.hasError());
}

TEST_F(AssetCacheInputMapTest,
       LoadInputMapCreatesAllSchemesCommandsAndBindings) {
  auto res = loadInputMap([](auto &node) {
    node["schemes"] = YAML::Node(YAML::NodeType::Sequence);
    node["commands"] = YAML::Node(YAML::NodeType::Sequence);
    node["bindings"] = YAML::Node(YAML::NodeType::Sequence);

    // schemes
    {
      YAML::Node item;
      item["name"] = "Gamepad";
      node["schemes"].push_back(item);
    }

    {
      YAML::Node item;
      item["name"] = "KBM";
      node["schemes"].push_back(item);
    }

    // commands
    {
      YAML::Node item;
      item["name"] = "Move";
      item["type"] = "axis-2d";
      node["commands"].push_back(item);
    }

    {
      YAML::Node item;
      item["name"] = "Look";
      item["type"] = "axis-2d";
      node["commands"].push_back(item);
    }

    {
      YAML::Node item;
      item["name"] = "Jump";
      item["type"] = "boolean";
      node["commands"].push_back(item);
    }

    // bindings
    {
      YAML::Node item;
      item["scheme"] = 0;
      item["command"] = 0;
      item["binding"]["x"] = std::vector{"KEY_A", "KEY_D"};
      item["binding"]["y"] = std::vector{"KEY_W", "KEY_S"};

      node["bindings"].push_back(item);
    }

    {
      YAML::Node item;
      item["scheme"] = 0;
      item["command"] = 1;
      item["binding"] = "MOUSE_MOVE";
      node["bindings"].push_back(item);
    }

    {
      YAML::Node item;
      item["scheme"] = 0;
      item["command"] = 2;
      item["binding"] = "KEY_SPACE";
      node["bindings"].push_back(item);
    }

    {
      YAML::Node item;
      item["scheme"] = 1;
      item["command"] = 0;
      item["binding"]["x"] = "GAMEPAD_LEFT_X";
      item["binding"]["y"] = "GAMEPAD_LEFT_Y";
      node["bindings"].push_back(item);
    }

    {
      YAML::Node item;
      item["scheme"] = 1;
      item["command"] = 1;
      item["binding"]["x"] = "GAMEPAD_RIGHT_X";
      item["binding"]["y"] = "GAMEPAD_RIGHT_Y";
      node["bindings"].push_back(item);
    }

    {
      YAML::Node item;
      item["scheme"] = 1;
      item["command"] = 2;
      item["binding"] = "GAMEPAD_SOUTH";
      node["bindings"].push_back(item);
    }
  });

  EXPECT_FALSE(res.hasError());
  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasWarnings());

  const auto &inputMap =
      cache.getRegistry().getInputMaps().getAsset(res.getData());

  EXPECT_EQ(inputMap.data.schemes.size(), 2);
  EXPECT_EQ(inputMap.data.schemes.at(0).name, "Gamepad");
  EXPECT_EQ(inputMap.data.schemes.at(1).name, "KBM");

  EXPECT_EQ(inputMap.data.commands.size(), 3);
  EXPECT_EQ(inputMap.data.commands.at(0).name, "Move");
  EXPECT_EQ(inputMap.data.commands.at(0).type, quoll::InputDataType::Axis2d);

  EXPECT_EQ(inputMap.data.commands.at(1).name, "Look");
  EXPECT_EQ(inputMap.data.commands.at(1).type, quoll::InputDataType::Axis2d);

  EXPECT_EQ(inputMap.data.commands.at(2).name, "Jump");
  EXPECT_EQ(inputMap.data.commands.at(2).type, quoll::InputDataType::Boolean);

  EXPECT_EQ(inputMap.data.bindings.size(), 6);

  auto &bindings = inputMap.data.bindings;
  EXPECT_EQ(bindings.at(0).scheme, 0);
  EXPECT_EQ(bindings.at(0).command, 0);
  {
    auto value2d = std::get<quoll::InputMapAxis2dValue>(bindings.at(0).value);

    auto x = std::get<quoll::InputMapAxisSegment>(value2d.x);
    auto y = std::get<quoll::InputMapAxisSegment>(value2d.y);

    EXPECT_EQ(x.at(0), quoll::input::get("KEY_A"));
    EXPECT_EQ(x.at(1), quoll::input::get("KEY_D"));

    EXPECT_EQ(y.at(0), quoll::input::get("KEY_W"));
    EXPECT_EQ(y.at(1), quoll::input::get("KEY_S"));
  }

  EXPECT_EQ(bindings.at(1).scheme, 0);
  EXPECT_EQ(bindings.at(1).command, 1);
  EXPECT_EQ(std::get<quoll::InputMapValue>(bindings.at(1).value),
            quoll::input::get("MOUSE_MOVE"));

  EXPECT_EQ(bindings.at(2).scheme, 0);
  EXPECT_EQ(bindings.at(2).command, 2);
  EXPECT_EQ(std::get<quoll::InputMapValue>(bindings.at(2).value),
            quoll::input::get("KEY_SPACE"));

  EXPECT_EQ(bindings.at(3).scheme, 1);
  EXPECT_EQ(bindings.at(3).command, 0);
  {
    auto value2d = std::get<quoll::InputMapAxis2dValue>(bindings.at(3).value);

    auto x = std::get<quoll::InputMapValue>(value2d.x);
    auto y = std::get<quoll::InputMapValue>(value2d.y);

    EXPECT_EQ(x, quoll::input::get("GAMEPAD_LEFT_X"));
    EXPECT_EQ(y, quoll::input::get("GAMEPAD_LEFT_Y"));
  }

  EXPECT_EQ(bindings.at(4).scheme, 1);
  EXPECT_EQ(bindings.at(4).command, 1);
  {
    auto value2d = std::get<quoll::InputMapAxis2dValue>(bindings.at(4).value);

    auto x = std::get<quoll::InputMapValue>(value2d.x);
    auto y = std::get<quoll::InputMapValue>(value2d.y);

    EXPECT_EQ(x, quoll::input::get("GAMEPAD_RIGHT_X"));
    EXPECT_EQ(y, quoll::input::get("GAMEPAD_RIGHT_Y"));
  }

  EXPECT_EQ(bindings.at(5).scheme, 1);
  EXPECT_EQ(bindings.at(5).command, 2);
  EXPECT_EQ(std::get<quoll::InputMapValue>(bindings.at(5).value),
            quoll::input::get("GAMEPAD_SOUTH"));
}

TEST_F(AssetCacheInputMapTest,
       UpdatesExistingInputMapIfAssetWithUuidAlreadyExists) {
  auto uuid = quoll::Uuid::generate();
  auto result = loadInputMap([](auto &node) {}, uuid);

  EXPECT_FALSE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_TRUE(result.hasData());

  auto handle = result.getData();

  {
    auto &inputMap = cache.getRegistry().getInputMaps().getAsset(handle);
    EXPECT_EQ(inputMap.data.schemes.size(), 1);
  }

  {
    auto result = loadInputMap(
        [](auto &node) {
          YAML::Node item;
          item["name"] = "test2";
          node["schemes"].push_back(item);
        },
        uuid);

    EXPECT_EQ(result.getData(), handle);

    auto &inputMap = cache.getRegistry().getInputMaps().getAsset(handle);
    EXPECT_EQ(inputMap.data.schemes.size(), 2);
  }
}
