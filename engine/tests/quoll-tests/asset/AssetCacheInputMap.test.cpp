#include "quoll/core/Base.h"
#include <random>
#include <GLFW/glfw3.h>

#include "quoll/core/Version.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/yaml/Yaml.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheTestBase.h"

static const quoll::Path FilePath =
    AssetCacheTestBase::CachePath / "test.inputmap";

class AssetCacheInputMapTest : public AssetCacheTestBase {
public:
  quoll::Result<quoll::InputMapAssetHandle>
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

  {
    auto res = loadInputMap([](auto &node) {
      YAML::Node item;
      item["scheme"] = 0;
      item["command"] = 0;
      item["binding"].push_back("A");
      item["binding"].push_back("B");
      item["binding"].push_back("C");
      item["binding"].push_back("D");

      node["bindings"].push_back(item);
    });
    EXPECT_TRUE(res.hasError());
  }
}

TEST_F(AssetCacheInputMapTest,
       LoadInputMapFailsIfBindingValueDoesNotMatchBooleanCommandType) {
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
    binding["binding"].push_back("A");
    binding["binding"].push_back("B");
    binding["binding"].push_back("C");
    binding["binding"].push_back("D");

    node["bindings"].push_back(binding);
  });
  EXPECT_TRUE(res.hasError());
}

TEST_F(AssetCacheInputMapTest,
       LoadInputMapFailsIfBindingValueDoesNotMatchAxis2DCommandType) {
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
      binding["binding"] = "A";
      node["bindings"].push_back(binding);
    });
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
      binding["binding"].push_back("A");
      node["bindings"].push_back(binding);
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

TEST_F(AssetCacheInputMapTest, LoadInputMapAllowsCreatingBindingValueWithNull) {
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
  EXPECT_FALSE(res.hasError());
  EXPECT_TRUE(res.hasData());
}

TEST_F(AssetCacheInputMapTest, LoadInputMapCreatesInputMapAsset) {
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
      item["binding"] = std::vector{"W", "A", "S", "D"};
      node["bindings"].push_back(item);
    }

    // TODO: Use mouse axis here
    {
      YAML::Node item;
      item["scheme"] = 0;
      item["command"] = 1;
      item["binding"] = std::vector{"W", "A", "S", "D"};
      node["bindings"].push_back(item);
    }

    {
      YAML::Node item;
      item["scheme"] = 0;
      item["command"] = 2;
      item["binding"] = "SPACE";
      node["bindings"].push_back(item);
    }

    // TODO: Use gamepad axis and buttons here
    {
      YAML::Node item;
      item["scheme"] = 1;
      item["command"] = 0;
      item["binding"] = std::vector{"W", "A", "S", "D"};
      node["bindings"].push_back(item);
    }

    {
      YAML::Node item;
      item["scheme"] = 1;
      item["command"] = 1;
      item["binding"] = std::vector{"W", "A", "S", "D"};
      node["bindings"].push_back(item);
    }

    {
      YAML::Node item;
      item["scheme"] = 1;
      item["command"] = 2;
      item["binding"] = "SPACE";
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
  EXPECT_EQ(inputMap.data.commands.at(0).type,
            quoll::InputMapCommandType::Axis2d);

  EXPECT_EQ(inputMap.data.commands.at(1).name, "Look");
  EXPECT_EQ(inputMap.data.commands.at(1).type,
            quoll::InputMapCommandType::Axis2d);

  EXPECT_EQ(inputMap.data.commands.at(2).name, "Jump");
  EXPECT_EQ(inputMap.data.commands.at(2).type,
            quoll::InputMapCommandType::Boolean);

  EXPECT_EQ(inputMap.data.bindings.size(), 6);
  EXPECT_EQ(inputMap.data.bindings.at(0).scheme, 0);
  EXPECT_EQ(inputMap.data.bindings.at(0).command, 0);
  EXPECT_EQ(inputMap.data.bindings.at(0).binding.size(), 4);
  EXPECT_EQ(inputMap.data.bindings.at(0).binding.at(0).key, GLFW_KEY_W);
  EXPECT_EQ(inputMap.data.bindings.at(0).binding.at(1).key, GLFW_KEY_A);
  EXPECT_EQ(inputMap.data.bindings.at(0).binding.at(2).key, GLFW_KEY_S);
  EXPECT_EQ(inputMap.data.bindings.at(0).binding.at(3).key, GLFW_KEY_D);

  EXPECT_EQ(inputMap.data.bindings.at(1).scheme, 0);
  EXPECT_EQ(inputMap.data.bindings.at(1).command, 1);
  EXPECT_EQ(inputMap.data.bindings.at(1).binding.size(), 4);
  EXPECT_EQ(inputMap.data.bindings.at(1).binding.at(0).key, GLFW_KEY_W);
  EXPECT_EQ(inputMap.data.bindings.at(1).binding.at(1).key, GLFW_KEY_A);
  EXPECT_EQ(inputMap.data.bindings.at(1).binding.at(2).key, GLFW_KEY_S);
  EXPECT_EQ(inputMap.data.bindings.at(1).binding.at(3).key, GLFW_KEY_D);

  EXPECT_EQ(inputMap.data.bindings.at(2).scheme, 0);
  EXPECT_EQ(inputMap.data.bindings.at(2).command, 2);
  EXPECT_EQ(inputMap.data.bindings.at(2).binding.size(), 1);
  EXPECT_EQ(inputMap.data.bindings.at(2).binding.at(0).key, GLFW_KEY_SPACE);

  EXPECT_EQ(inputMap.data.bindings.at(3).scheme, 1);
  EXPECT_EQ(inputMap.data.bindings.at(3).command, 0);
  EXPECT_EQ(inputMap.data.bindings.at(3).binding.at(0).key, GLFW_KEY_W);
  EXPECT_EQ(inputMap.data.bindings.at(3).binding.at(1).key, GLFW_KEY_A);
  EXPECT_EQ(inputMap.data.bindings.at(3).binding.at(2).key, GLFW_KEY_S);
  EXPECT_EQ(inputMap.data.bindings.at(3).binding.at(3).key, GLFW_KEY_D);

  EXPECT_EQ(inputMap.data.bindings.at(4).scheme, 1);
  EXPECT_EQ(inputMap.data.bindings.at(4).command, 1);
  EXPECT_EQ(inputMap.data.bindings.at(4).binding.at(0).key, GLFW_KEY_W);
  EXPECT_EQ(inputMap.data.bindings.at(4).binding.at(1).key, GLFW_KEY_A);
  EXPECT_EQ(inputMap.data.bindings.at(4).binding.at(2).key, GLFW_KEY_S);
  EXPECT_EQ(inputMap.data.bindings.at(4).binding.at(3).key, GLFW_KEY_D);

  EXPECT_EQ(inputMap.data.bindings.at(5).scheme, 1);
  EXPECT_EQ(inputMap.data.bindings.at(5).command, 2);
  EXPECT_EQ(inputMap.data.bindings.at(5).binding.size(), 1);
  EXPECT_EQ(inputMap.data.bindings.at(5).binding.at(0).key, GLFW_KEY_SPACE);
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
