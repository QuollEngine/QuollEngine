#include "quoll/core/Base.h"
#include "quoll/core/Id.h"
#include "quoll/scene/EnvironmentLighting.h"
#include "quoll/scene/EnvironmentSkybox.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/Parent.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/io/SceneIO.h"
#include "quoll/io/EntitySerializer.h"

#include "quoll-tests/Testing.h"

const quoll::Path ScenePath =
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

  quoll::SceneAssetHandle
  createSceneAsset(const std::vector<YAML::Node> &entities = {}) {
    YAML::Node root;
    root["name"] = "TestScene";
    root["version"] = "0.1";

    YAML::Node zoneNode;
    zoneNode["name"] = "TestZone";
    zoneNode["entities"] = "entities";
    zoneNode["startingCamera"] = 3;
    root["zones"][0] = zoneNode;
    root["entities"] = entities;

    quoll::AssetData<quoll::SceneAsset> asset{};
    asset.name = "Scene";
    asset.data.data = root;

    return assetRegistry.getScenes().addAsset(asset);
  }

  YAML::Node getSceneYaml(quoll::SceneAssetHandle handle) {
    return assetRegistry.getScenes().getAsset(handle).data.data;
  }

public:
  quoll::AssetRegistry assetRegistry;
  quoll::Scene scene;
  quoll::SceneIO sceneIO;
};

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfNodeDoesNotHaveId) {
  YAML::Node node;

  auto handle = createSceneAsset({node});
  sceneIO.loadScene(handle);

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

    auto handle = createSceneAsset({node});
    sceneIO.loadScene(handle);

    EXPECT_EQ(scene.entityDatabase.getEntityCount(), 2);
  }
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdIsZero) {
  YAML::Node node;
  node["id"] = 0;

  auto handle = createSceneAsset({node});
  sceneIO.loadScene(handle);

  EXPECT_EQ(scene.entityDatabase.getEntityCount(), 2);
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdIsNegative) {
  YAML::Node node;
  node["id"] = -1;

  auto handle = createSceneAsset({node});
  sceneIO.loadScene(handle);

  EXPECT_EQ(scene.entityDatabase.getEntityCount(), 2);
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdAlreadyExists) {
  YAML::Node node;
  node["id"] = 50;

  auto handle = createSceneAsset({node, node});
  sceneIO.loadScene(handle);

  EXPECT_EQ(scene.entityDatabase.getEntityCount(), 3);
}

TEST_F(SceneIOTest, LoadsSceneFileWithManyEntities) {
  static constexpr u64 NumEntities = 9;

  std::vector<YAML::Node> nodes(NumEntities);

  for (u64 i = 1; i < NumEntities + 1; ++i) {
    YAML::Node node;
    node["id"] = i;
    nodes.push_back(node);
  }

  auto handle = createSceneAsset(nodes);
  const auto &entities = sceneIO.loadScene(handle);

  EXPECT_GT(scene.entityDatabase.getEntityCount(), NumEntities);
  EXPECT_GT(scene.entityDatabase.getEntityCount(), entities.size() + 1);
  for (auto entity : entities) {
    EXPECT_TRUE(scene.entityDatabase.has<quoll::Id>(entity));
  }
}

TEST_F(SceneIOTest, LoadingSetsParentsProperly) {
  static constexpr u64 NumEntities = 9;

  std::vector<YAML::Node> nodes(NumEntities);

  for (u64 i = 1; i < NumEntities + 1; ++i) {
    // set parent to next entity
    // to make sure that parent entities
    // are loaded after child ones

    YAML::Node node;
    node["id"] = i;
    node["transform"]["parent"] = i + 1;
    nodes.push_back(node);
  }

  auto handle = createSceneAsset(nodes);
  const auto &entities = sceneIO.loadScene(handle);

  EXPECT_GT(scene.entityDatabase.getEntityCount(), entities.size() + 1);
  EXPECT_EQ(scene.entityDatabase.getEntityCountForComponent<quoll::Parent>(),
            entities.size() - 1);
}

TEST_F(SceneIOTest, CreatesDummyCameraComponentOnConstruct) {
  EXPECT_TRUE(scene.entityDatabase.exists(scene.dummyCamera));
  EXPECT_TRUE(scene.entityDatabase.has<quoll::Camera>(scene.dummyCamera));
  EXPECT_TRUE(
      scene.entityDatabase.has<quoll::PerspectiveLens>(scene.dummyCamera));
}

TEST_F(SceneIOTest, SetsInitialCameraAsTheActiveCameraOnLoad) {
  auto handle = createSceneAsset();

  {
    auto entity = scene.entityDatabase.create();
    scene.entityDatabase.set<quoll::Id>(entity, {3});
    scene.entityDatabase.set<quoll::PerspectiveLens>(entity, {});

    quoll::detail::EntitySerializer serializer(assetRegistry,
                                               scene.entityDatabase);

    auto entityNode = serializer.serialize(entity);

    createSceneFileWithEntity({entityNode.getData()});
  }

  sceneIO.loadScene(handle);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.activeCamera));
  EXPECT_TRUE(
      scene.entityDatabase.has<quoll::PerspectiveLens>(scene.activeCamera));
}

TEST_F(SceneIOTest,
       SetsDummyCameraAsTheActiveCameraOnLoadIfNoCameraExistsForTheScene) {
  auto handle = createSceneAsset();
  sceneIO.loadScene(handle);
  EXPECT_EQ(scene.activeCamera, scene.dummyCamera);
}

TEST_F(SceneIOTest,
       CreatesEmptyEnvironmentEntityOnLoadIfSceneEnvironmentDoesNotExist) {
  auto handle = createSceneAsset();
  sceneIO.loadScene(handle);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.activeEnvironment));
  EXPECT_FALSE(scene.entityDatabase.has<quoll::EnvironmentSkybox>(
      scene.activeEnvironment));
  EXPECT_FALSE(scene.entityDatabase.has<quoll::EnvironmentLightingSkyboxSource>(
      scene.activeEnvironment));
}

TEST_F(SceneIOTest,
       CreatesEmptyEnvironmentEntityOnLoadIfSceneEnvironmentIsInvalid) {
  auto handle = createSceneAsset();
  auto zoneNode = getSceneYaml(handle)["zones"][0];

  std::vector<YAML::Node> invalidNodes{
      YAML::Node(YAML::NodeType::Undefined), YAML::Node(YAML::NodeType::Null),
      YAML::Node(YAML::NodeType::Map), YAML::Node(YAML::NodeType::Sequence),
      YAML::Node(YAML::NodeType::Scalar)};

  for (const auto &invalidNode : invalidNodes) {
    zoneNode["environment"] = invalidNode;
    sceneIO.loadScene(handle);

    EXPECT_TRUE(scene.entityDatabase.exists(scene.activeEnvironment));
    EXPECT_FALSE(scene.entityDatabase.has<quoll::EnvironmentSkybox>(
        scene.activeEnvironment));
    EXPECT_FALSE(
        scene.entityDatabase.has<quoll::EnvironmentLightingSkyboxSource>(
            scene.activeEnvironment));
  }
}

TEST_F(
    SceneIOTest,
    CreatesEmptyEnvironmentEntityOnLoadIfEnvironmentDoesNotPointToValidEntity) {
  auto handle = createSceneAsset();
  auto zoneNode = getSceneYaml(handle)["zones"][0];
  zoneNode["environment"] = 100;

  sceneIO.loadScene(handle);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.activeEnvironment));
  EXPECT_FALSE(scene.entityDatabase.has<quoll::EnvironmentSkybox>(
      scene.activeEnvironment));
  EXPECT_FALSE(scene.entityDatabase.has<quoll::EnvironmentLightingSkyboxSource>(
      scene.activeEnvironment));
}

TEST_F(
    SceneIOTest,
    CreatesEnvironmentEntityWithoutSkyboxComponentsOnLoadIfEnvironmentEntityHasNoSkybox) {
  YAML::Node envEntity;
  envEntity["id"] = 125;

  auto handle = createSceneAsset({envEntity});
  auto sceneNode = getSceneYaml(handle);

  auto zoneNode = sceneNode["zones"][0];

  zoneNode["environment"] = 125;
  sceneIO.loadScene(handle);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.activeEnvironment));
  EXPECT_EQ(scene.entityDatabase.get<quoll::Id>(scene.activeEnvironment).id,
            125);
  EXPECT_FALSE(scene.entityDatabase.has<quoll::EnvironmentSkybox>(
      scene.activeEnvironment));
}

TEST_F(
    SceneIOTest,
    CreatesEnvironmentEntityWithoutEnvironmentLightingComponentsOnLoadIfEnvironmentEntityHasNoEnvironmentLighting) {
  YAML::Node envEntity;
  envEntity["id"] = 125;

  auto handle = createSceneAsset({envEntity});
  auto sceneNode = getSceneYaml(handle);

  auto zoneNode = sceneNode["zones"][0];

  zoneNode["environment"] = 125;
  sceneIO.loadScene(handle);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.activeEnvironment));
  EXPECT_EQ(scene.entityDatabase.get<quoll::Id>(scene.activeEnvironment).id,
            125);
  EXPECT_FALSE(scene.entityDatabase.has<quoll::EnvironmentLightingSkyboxSource>(
      scene.activeEnvironment));
}

TEST_F(
    SceneIOTest,
    CreatesEnvironmentEntityWithEnvironmentLightingSourceSkyboxIfLightingSourceIsSkybox) {
  YAML::Node envEntity;
  envEntity["id"] = 125;
  envEntity["environmentLighting"]["source"] = "skybox";

  auto handle = createSceneAsset({envEntity});
  auto sceneNode = getSceneYaml(handle);

  auto zoneNode = sceneNode["zones"][0];

  zoneNode["environment"] = 125;
  sceneIO.loadScene(handle);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.activeEnvironment));
  EXPECT_EQ(scene.entityDatabase.get<quoll::Id>(scene.activeEnvironment).id,
            125);
  EXPECT_TRUE(scene.entityDatabase.has<quoll::EnvironmentLightingSkyboxSource>(
      scene.activeEnvironment));
}
