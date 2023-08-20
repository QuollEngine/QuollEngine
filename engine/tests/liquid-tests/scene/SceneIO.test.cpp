#include "liquid/core/Base.h"
#include "liquid-tests/Testing.h"

#include "liquid/entity/EntityDatabase.h"
#include "liquid/scene/SceneIO.h"
#include "liquid/scene/private/EntitySerializer.h"

const liquid::Path ScenePath =
    std::filesystem::current_path() / "scene-io-test" / "main.scene";

class SceneIOTest : public ::testing::Test {

public:
  SceneIOTest() : sceneIO(assetRegistry, scene) {}

  void SetUp() override {
    TearDown();
    std::filesystem::create_directory(ScenePath.parent_path());
    std::ofstream stream(ScenePath);
    YAML::Node root;
    root["name"] = "TestScene";
    root["version"] = "0.1";

    YAML::Node zoneNode;
    zoneNode["name"] = "TestZone";
    zoneNode["startingCamera"] = 3;
    root["zones"][0] = zoneNode;

    stream << root;
    stream.close();
  }

  void TearDown() override {
    std::filesystem::remove_all(ScenePath.parent_path());
  }

  YAML::Node loadSceneFile(liquid::Path path) {
    std::ifstream stream(path);
    auto node = YAML::Load(stream);

    stream.close();
    return node;
  }

  void saveSceneFile(YAML::Node node, liquid::Path path) {
    std::ofstream stream(path);
    stream << node;
    stream.close();
  }

  void createSceneFileWithEntity(const std::vector<YAML::Node> &entities) {
    YAML::Node root;
    root["name"] = "TestScene";
    root["version"] = "0.1";

    YAML::Node zoneNode;
    zoneNode["name"] = "TestZone";
    zoneNode["entities"] = "entities";
    zoneNode["startingCamera"] = 3;
    root["zones"][0] = zoneNode;
    root["entities"] = entities;

    std::ofstream stream(ScenePath);
    stream << root;
    stream.close();
  }

public:
  liquid::AssetRegistry assetRegistry;
  liquid::Scene scene;
  liquid::SceneIO sceneIO;
};

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfNodeDoesNotHaveId) {
  YAML::Node node;

  createSceneFileWithEntity({node});
  sceneIO.loadScene(ScenePath);

  EXPECT_EQ(scene.entityDatabase.getEntityCount(), 2);
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdIsInvalid) {
  std::vector<YAML::Node> invalidNodes{
      YAML::Node(YAML::NodeType::Undefined),
      YAML::Node(YAML::NodeType::Null),
      YAML::Node(YAML::NodeType::Map),
      YAML::Node(YAML::NodeType::Sequence),
      YAML::Node(YAML::NodeType::Scalar),
  };

  for (const auto &invalidNode : invalidNodes) {
    YAML::Node node;
    node["id"] = invalidNode;
    createSceneFileWithEntity({node});
    sceneIO.loadScene(ScenePath);

    EXPECT_EQ(scene.entityDatabase.getEntityCount(), 2);
  }
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdIsZero) {
  YAML::Node node;
  node["id"] = 0;
  createSceneFileWithEntity({node});

  sceneIO.loadScene(ScenePath);

  EXPECT_EQ(scene.entityDatabase.getEntityCount(), 2);
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdIsNegative) {
  YAML::Node node;
  node["id"] = -1;
  createSceneFileWithEntity({node});

  sceneIO.loadScene(ScenePath);

  EXPECT_EQ(scene.entityDatabase.getEntityCount(), 2);
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdAlreadyExists) {
  YAML::Node node;
  node["id"] = 50;

  createSceneFileWithEntity({node, node});

  sceneIO.loadScene(ScenePath);

  EXPECT_EQ(scene.entityDatabase.getEntityCount(), 3);
}

TEST_F(SceneIOTest, LoadsSceneFileWithManyEntities) {
  static constexpr uint64_t NumEntities = 9;

  std::vector<YAML::Node> nodes(NumEntities);

  for (uint64_t i = 1; i < NumEntities + 1; ++i) {
    YAML::Node node;
    node["id"] = i;
    nodes.push_back(node);
  }

  createSceneFileWithEntity(nodes);

  const auto &entities = sceneIO.loadScene(ScenePath);

  EXPECT_GT(scene.entityDatabase.getEntityCount(), NumEntities);
  EXPECT_GT(scene.entityDatabase.getEntityCount(), entities.size() + 1);
  for (auto entity : entities) {
    EXPECT_TRUE(scene.entityDatabase.has<liquid::Id>(entity));
  }
}

TEST_F(SceneIOTest, LoadingSetsParentsProperly) {
  static constexpr uint64_t NumEntities = 9;

  std::vector<YAML::Node> nodes(NumEntities);

  for (uint64_t i = 1; i < NumEntities + 1; ++i) {
    // set parent to next entity
    // to make sure that parent entities
    // are loaded after child ones

    YAML::Node node;
    node["id"] = i;
    node["transform"]["parent"] = i + 1;
    nodes.push_back(node);
  }

  createSceneFileWithEntity(nodes);

  const auto &entities = sceneIO.loadScene(ScenePath);

  EXPECT_GT(scene.entityDatabase.getEntityCount(), entities.size() + 1);
  EXPECT_EQ(scene.entityDatabase.getEntityCountForComponent<liquid::Parent>(),
            entities.size() - 1);
}

TEST_F(SceneIOTest, CreatesDummyCameraComponentOnConstruct) {
  EXPECT_TRUE(scene.entityDatabase.exists(scene.dummyCamera));
  EXPECT_TRUE(scene.entityDatabase.has<liquid::Camera>(scene.dummyCamera));
  EXPECT_TRUE(
      scene.entityDatabase.has<liquid::PerspectiveLens>(scene.dummyCamera));
}

TEST_F(SceneIOTest, SetsInitialCameraAsTheActiveCameraOnLoad) {
  {
    auto entity = scene.entityDatabase.create();
    scene.entityDatabase.set<liquid::Id>(entity, {3});
    scene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});

    liquid::detail::EntitySerializer serializer(assetRegistry,
                                                scene.entityDatabase);

    auto entityNode = serializer.serialize(entity);

    createSceneFileWithEntity({entityNode.getData()});
  }

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.activeCamera));
  EXPECT_TRUE(
      scene.entityDatabase.has<liquid::PerspectiveLens>(scene.activeCamera));
}

TEST_F(SceneIOTest,
       SetsDummyCameraAsTheActiveCameraOnLoadIfNoCameraExistsForTheScene) {
  sceneIO.loadScene(ScenePath);
  EXPECT_EQ(scene.activeCamera, scene.dummyCamera);
}

TEST_F(SceneIOTest,
       CreatesEmptyEnvironmentEntityOnLoadIfSceneEnvironmentDoesNotExist) {
  auto node = loadSceneFile(ScenePath);
  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  EXPECT_FALSE(
      scene.entityDatabase.has<liquid::EnvironmentSkybox>(scene.environment));
  EXPECT_FALSE(
      scene.entityDatabase.has<liquid::EnvironmentLightingSkyboxSource>(
          scene.environment));
}

TEST_F(SceneIOTest,
       CreatesEmptyEnvironmentEntityOnLoadIfSceneEnvironmentIsNull) {
  auto node = loadSceneFile(ScenePath);
  auto zoneNode = node["zones"][0];
  zoneNode["environment"] = YAML::Node(YAML::NodeType::Null);
  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  EXPECT_FALSE(
      scene.entityDatabase.has<liquid::EnvironmentSkybox>(scene.environment));
  EXPECT_FALSE(
      scene.entityDatabase.has<liquid::EnvironmentLightingSkyboxSource>(
          scene.environment));
}

TEST_F(SceneIOTest,
       CreatesEmptyEnvironmentEntityOnLoadIfEnvironmentFieldIsNotAMap) {
  auto node = loadSceneFile(ScenePath);
  auto zoneNode = node["zones"][0];
  zoneNode["environment"] = YAML::Node(YAML::NodeType::Scalar);
  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  EXPECT_FALSE(
      scene.entityDatabase.has<liquid::EnvironmentSkybox>(scene.environment));
  EXPECT_FALSE(
      scene.entityDatabase.has<liquid::EnvironmentLightingSkyboxSource>(
          scene.environment));
}

TEST_F(SceneIOTest,
       CreatesEnvironmentEntityWithoutSkyboxComponentsOnLoadIfNoSkyboxField) {
  auto node = loadSceneFile(ScenePath);
  auto zoneNode = node["zones"][0];
  zoneNode["environment"] = YAML::Node(YAML::NodeType::Map);
  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  EXPECT_FALSE(
      scene.entityDatabase.has<liquid::EnvironmentSkybox>(scene.environment));
}

TEST_F(
    SceneIOTest,
    CreatesEnvironmentEntityWithoutSkyboxComponentsOnLoadIfSkyboxFieldIsNotMap) {
  auto node = loadSceneFile(ScenePath);
  auto zoneNode = node["zones"][0];
  zoneNode["environment"] = YAML::Node(YAML::NodeType::Map);
  zoneNode["environment"]["skybox"] = YAML::Node(YAML::NodeType::Scalar);
  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  EXPECT_FALSE(
      scene.entityDatabase.has<liquid::EnvironmentSkybox>(scene.environment));
}

TEST_F(
    SceneIOTest,
    CreatesEnvironmentEntityWithoutSkyboxComponentsOnLoadIfSkyboxFieldIsNull) {
  auto node = loadSceneFile(ScenePath);
  auto zoneNode = node["zones"][0];
  zoneNode["environment"] = YAML::Node(YAML::NodeType::Map);
  zoneNode["environment"]["skybox"] = YAML::Null;
  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  EXPECT_FALSE(
      scene.entityDatabase.has<liquid::EnvironmentSkybox>(scene.environment));
}

TEST_F(SceneIOTest,
       CreatesEnvironmentEntityWithNoSkyboxComponentsIfSkyboxTypeIsUnknown) {
  auto node = loadSceneFile(ScenePath);
  auto zoneNode = node["zones"][0];
  zoneNode["environment"] = YAML::Node(YAML::NodeType::Map);
  zoneNode["environment"]["skybox"]["type"] = "something-else";
  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  EXPECT_FALSE(
      scene.entityDatabase.has<liquid::EnvironmentSkybox>(scene.environment));
}

TEST_F(SceneIOTest,
       CreatesEnvironmentEntityWithSkyboxColorOnLoadIfSkyboxTypeIsColor) {
  auto node = loadSceneFile(ScenePath);
  auto zoneNode = node["zones"][0];
  zoneNode["environment"]["skybox"]["type"] = "color";
  zoneNode["environment"]["skybox"]["color"] =
      glm::vec4{0.5f, 0.2f, 0.3f, 1.0f};
  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  EXPECT_TRUE(
      scene.entityDatabase.has<liquid::EnvironmentSkybox>(scene.environment));
  EXPECT_EQ(
      scene.entityDatabase.get<liquid::EnvironmentSkybox>(scene.environment)
          .type,
      liquid::EnvironmentSkyboxType::Color);
  EXPECT_EQ(
      scene.entityDatabase.get<liquid::EnvironmentSkybox>(scene.environment)
          .texture,
      liquid::EnvironmentAssetHandle::Null);
  EXPECT_EQ(
      scene.entityDatabase.get<liquid::EnvironmentSkybox>(scene.environment)
          .color,
      glm::vec4(0.5f, 0.2f, 0.3f, 1.0f));
}

TEST_F(
    SceneIOTest,
    CreatesEnvironmentEntityWithBlackColorOnLoadIfSkyboxTypeIsColorButColorValueIsNotSequence) {
  auto node = loadSceneFile(ScenePath);
  auto zoneNode = node["zones"][0];
  zoneNode["environment"]["skybox"]["type"] = "color";
  zoneNode["environment"]["skybox"]["color"] =
      YAML::Node(YAML::NodeType::Scalar);
  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  EXPECT_TRUE(
      scene.entityDatabase.has<liquid::EnvironmentSkybox>(scene.environment));
  EXPECT_EQ(
      scene.entityDatabase.get<liquid::EnvironmentSkybox>(scene.environment)
          .type,
      liquid::EnvironmentSkyboxType::Color);
  EXPECT_EQ(
      scene.entityDatabase.get<liquid::EnvironmentSkybox>(scene.environment)
          .texture,
      liquid::EnvironmentAssetHandle::Null);
  EXPECT_EQ(
      scene.entityDatabase.get<liquid::EnvironmentSkybox>(scene.environment)
          .color,
      glm::vec4(0.0f));
}

TEST_F(
    SceneIOTest,
    CreatesEnvironmentEntityWithBlackColorOnLoadIfSkyboxTypeIsColorButColorValueIsNotVec4) {
  auto node = loadSceneFile(ScenePath);
  auto zoneNode = node["zones"][0];
  zoneNode["environment"]["skybox"]["type"] = "color";
  zoneNode["environment"]["skybox"]["color"][0] = 10.5f;

  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  EXPECT_TRUE(
      scene.entityDatabase.has<liquid::EnvironmentSkybox>(scene.environment));
  EXPECT_EQ(
      scene.entityDatabase.get<liquid::EnvironmentSkybox>(scene.environment)
          .type,
      liquid::EnvironmentSkyboxType::Color);
  EXPECT_EQ(
      scene.entityDatabase.get<liquid::EnvironmentSkybox>(scene.environment)
          .texture,
      liquid::EnvironmentAssetHandle::Null);
  EXPECT_EQ(
      scene.entityDatabase.get<liquid::EnvironmentSkybox>(scene.environment)
          .color,
      glm::vec4(0.0f));
}

TEST_F(
    SceneIOTest,
    CreatesEnvironmentEntityWithNoSkyboxTextureIfTypeIsTextureButEnvironmentAssetDoesNotExist) {
  auto node = loadSceneFile(ScenePath);
  auto zoneNode = node["zones"][0];
  zoneNode["environment"] = YAML::Node(YAML::NodeType::Map);
  zoneNode["environment"]["skybox"]["type"] = "texture";
  zoneNode["environment"]["skybox"]["texture"] = "non-existent-file.hdr";
  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  EXPECT_FALSE(
      scene.entityDatabase.has<liquid::EnvironmentSkybox>(scene.environment));
}

TEST_F(
    SceneIOTest,
    CreatesEnvironmentEntityWithSkyboxTextureIfTypeIsTextureAndEnvironmentAssetExists) {
  liquid::AssetData<liquid::EnvironmentAsset> asset{};
  asset.uuid = "test-env";
  auto handle = assetRegistry.getEnvironments().addAsset(asset);

  auto node = loadSceneFile(ScenePath);
  auto zoneNode = node["zones"][0];
  zoneNode["environment"]["skybox"]["type"] = "texture";
  zoneNode["environment"]["skybox"]["texture"] = "test-env";
  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  ASSERT_TRUE(
      scene.entityDatabase.has<liquid::EnvironmentSkybox>(scene.environment));
  EXPECT_EQ(
      scene.entityDatabase.get<liquid::EnvironmentSkybox>(scene.environment)
          .type,
      liquid::EnvironmentSkyboxType::Texture);
  EXPECT_EQ(
      scene.entityDatabase.get<liquid::EnvironmentSkybox>(scene.environment)
          .texture,
      handle);
}

TEST_F(
    SceneIOTest,
    CreatesEnvironmentEntityWithoutEnvironmentLightingComponentsOnLoadIfNoLightingFiled) {
  auto node = loadSceneFile(ScenePath);
  auto zoneNode = node["zones"][0];
  zoneNode["environment"] = YAML::Node(YAML::NodeType::Map);
  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  EXPECT_FALSE(
      scene.entityDatabase.has<liquid::EnvironmentLightingSkyboxSource>(
          scene.environment));
}

TEST_F(
    SceneIOTest,
    CreatesEnvironmentEntityWithoutEnvironmentLightingComponentsOnLoadIfLightingFieldIsNotMap) {
  auto node = loadSceneFile(ScenePath);
  auto zoneNode = node["zones"][0];
  zoneNode["environment"] = YAML::Node(YAML::NodeType::Map);
  zoneNode["environment"]["lighting"] = YAML::Node(YAML::NodeType::Scalar);
  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  EXPECT_FALSE(
      scene.entityDatabase.has<liquid::EnvironmentLightingSkyboxSource>(
          scene.environment));
}

TEST_F(
    SceneIOTest,
    CreatesEnvironmentEntityWithoutEnvironmentLightingComponentsOnLoadIfLightingFieldIsNull) {
  auto node = loadSceneFile(ScenePath);
  auto zoneNode = node["zones"][0];
  zoneNode["environment"] = YAML::Node(YAML::NodeType::Map);
  zoneNode["environment"]["lighting"] = YAML::Null;
  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  EXPECT_FALSE(
      scene.entityDatabase.has<liquid::EnvironmentLightingSkyboxSource>(
          scene.environment));
}

TEST_F(
    SceneIOTest,
    CreatesEnvironmentEntityWithNoEnvironmentLightingComponentOnLoadIfLightingSourceIsUnknown) {
  auto node = loadSceneFile(ScenePath);
  auto zoneNode = node["zones"][0];
  zoneNode["environment"] = YAML::Node(YAML::NodeType::Map);
  zoneNode["environment"]["lighting"]["source"] = "something-else";
  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  EXPECT_FALSE(
      scene.entityDatabase.has<liquid::EnvironmentLightingSkyboxSource>(
          scene.environment));
}

TEST_F(
    SceneIOTest,
    CreatesEnvironmentEntityWithEnvironmentLightingSourceSkyboxIfLightingSourceIsSkybox) {
  liquid::AssetData<liquid::EnvironmentAsset> asset{};
  asset.uuid = "test-env";
  auto handle = assetRegistry.getEnvironments().addAsset(asset);

  auto node = loadSceneFile(ScenePath);
  auto zoneNode = node["zones"][0];
  zoneNode["environment"]["lighting"]["source"] = "skybox";
  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  EXPECT_TRUE(scene.entityDatabase.has<liquid::EnvironmentLightingSkyboxSource>(
      scene.environment));
}
