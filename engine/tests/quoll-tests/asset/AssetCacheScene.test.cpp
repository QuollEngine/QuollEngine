#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/yaml/Yaml.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheTestBase.h"

class AssetCacheSceneTest : public AssetCacheTestBase {
public:
};

TEST_F(AssetCacheSceneTest, CreatesSceneFromSource) {
  auto uuid = quoll::Uuid::generate();
  auto filePath =
      cache.createSceneFromSource(FixturesPath / "test.scene", uuid);
  EXPECT_TRUE(filePath.hasData());
  EXPECT_FALSE(filePath.hasError());
  EXPECT_FALSE(filePath.hasWarnings());

  EXPECT_EQ(filePath.getData().filename().string().size(), 38);

  auto meta = cache.getAssetMeta(uuid);

  EXPECT_EQ(meta.type, quoll::AssetType::Scene);
  EXPECT_EQ(meta.name, "test.scene");
}

TEST_F(AssetCacheSceneTest, LoadSceneFailsIfTypeIsInvalid) {
  YAML::Node node;
  node["version"] = "0.1";
  node["type"] = "test";
  node["name"] = "test scene";
  node["zones"] = YAML::Node(YAML::NodeType::Sequence);
  node["entities"] = YAML::Node(YAML::NodeType::Sequence);

  auto uuid = quoll::Uuid::generate();

  std::ofstream stream(cache.getPathFromUuid(uuid));
  stream << node;
  stream.close();

  auto res = cache.loadScene(uuid);
  EXPECT_FALSE(res.hasData());
  EXPECT_TRUE(res.hasError());
  EXPECT_FALSE(res.hasWarnings());
}

TEST_F(AssetCacheSceneTest, LoadSceneFailsIfVersionIsInvalid) {
  YAML::Node node;
  node["version"] = "0.2";
  node["type"] = "scene";
  node["name"] = "test scene";
  node["zones"] = YAML::Node(YAML::NodeType::Sequence);
  node["entities"] = YAML::Node(YAML::NodeType::Sequence);

  auto uuid = quoll::Uuid::generate();

  std::ofstream stream(cache.getPathFromUuid(uuid));

  stream << node;
  stream.close();

  auto res = cache.loadScene(uuid);
  EXPECT_FALSE(res.hasData());
  EXPECT_TRUE(res.hasError());
  EXPECT_FALSE(res.hasWarnings());
}

TEST_F(AssetCacheSceneTest, LoadSceneFailsIfZonesFieldIsNotSequence) {
  std::vector<YAML::Node> invalidNodes{
      YAML::Node(YAML::NodeType::Null),
      YAML::Node(YAML::NodeType::Map),
      YAML::Node(YAML::NodeType::Scalar),
  };

  for (auto invNode : invalidNodes) {
    YAML::Node node;
    node["version"] = "0.1";
    node["type"] = "scene";
    node["name"] = "test scene";
    node["entities"] = YAML::Node(YAML::NodeType::Sequence);
    node["zones"] = invNode;

    auto uuid = quoll::Uuid::generate();

    std::ofstream stream(cache.getPathFromUuid(uuid));

    stream << node;
    stream.close();

    auto res = cache.loadScene(uuid);
    EXPECT_FALSE(res.hasData());
    EXPECT_TRUE(res.hasError());
    EXPECT_FALSE(res.hasWarnings());
  }
}

TEST_F(AssetCacheSceneTest, LoadSceneFailsIfEntitiesFieldIsNotSequence) {
  std::vector<YAML::Node> invalidNodes{
      YAML::Node(YAML::NodeType::Null),
      YAML::Node(YAML::NodeType::Map),
      YAML::Node(YAML::NodeType::Scalar),
  };

  for (auto invNode : invalidNodes) {
    YAML::Node node;
    node["version"] = "0.1";
    node["type"] = "scene";
    node["name"] = "test scene";
    node["zones"] = YAML::Node(YAML::NodeType::Sequence);
    node["entities"] = invNode;

    auto uuid = quoll::Uuid::generate();

    std::ofstream stream(cache.getPathFromUuid(uuid));
    stream << node;
    stream.close();

    auto res = cache.loadScene(uuid);
    EXPECT_FALSE(res.hasData());
    EXPECT_TRUE(res.hasError());
    EXPECT_FALSE(res.hasWarnings());
  }
}
