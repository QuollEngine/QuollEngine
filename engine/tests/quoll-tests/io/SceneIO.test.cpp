#include "quoll/core/Base.h"
#include "quoll/core/Id.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/io/EntitySerializer.h"
#include "quoll/io/SceneIO.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/EnvironmentLighting.h"
#include "quoll/scene/EnvironmentSkybox.h"
#include "quoll/scene/Parent.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/scene/Scene.h"
#include "quoll-tests/Testing.h"

const quoll::Path ScenePath =
    std::filesystem::current_path() / "scene-io-test" / "main.scene";

class SceneIOTest : public ::testing::Test {

public:
  SceneIOTest() : sceneIO(assetRegistry, scene) {
    quoll::EntityDatabase db;
    totalBuiltins = ecs_get_entities(db).alive_count;
  }

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

  inline i32 getCreatedEntities() const {
    return ecs_get_entities(scene.entityDatabase).alive_count - totalBuiltins;
  }

public:
  quoll::AssetRegistry assetRegistry;
  quoll::Scene scene;
  quoll::SceneIO sceneIO;
  i32 totalBuiltins = 0;
};

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfNodeDoesNotHaveId) {
  YAML::Node node;

  auto handle = createSceneAsset({node});
  sceneIO.loadScene(handle);

  EXPECT_EQ(getCreatedEntities(), 2);
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

    EXPECT_EQ(getCreatedEntities(), 2);
  }
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdIsZero) {
  YAML::Node node;
  node["id"] = 0;

  auto handle = createSceneAsset({node});
  sceneIO.loadScene(handle);

  EXPECT_EQ(getCreatedEntities(), 2);
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdIsNegative) {
  YAML::Node node;
  node["id"] = -1;

  auto handle = createSceneAsset({node});
  sceneIO.loadScene(handle);

  EXPECT_EQ(getCreatedEntities(), 2);
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdAlreadyExists) {
  YAML::Node node;
  node["id"] = 50;

  auto handle = createSceneAsset({node, node});
  sceneIO.loadScene(handle);

  EXPECT_EQ(getCreatedEntities(), 3);
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

  EXPECT_GT(ecs_get_entities(scene.entityDatabase).alive_count - totalBuiltins,
            NumEntities);
  EXPECT_GT(ecs_get_entities(scene.entityDatabase).alive_count - totalBuiltins,
            entities.size() + 1);

  for (auto entity : entities) {
    EXPECT_TRUE(entity.has<quoll::Id>());
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

  EXPECT_GT(ecs_get_entities(scene.entityDatabase).alive_count,
            entities.size() + 1);
  EXPECT_EQ(scene.entityDatabase.count<quoll::Parent>(), entities.size() - 1);
}

TEST_F(SceneIOTest, CreatesDummyCameraComponentOnConstruct) {
  auto dummyCamera = scene.dummyCamera;

  EXPECT_TRUE(dummyCamera.is_valid());
  EXPECT_TRUE(dummyCamera.has<quoll::Camera>());
  EXPECT_TRUE(dummyCamera.has<quoll::PerspectiveLens>());
}

TEST_F(SceneIOTest, SetsInitialCameraAsTheActiveCameraOnLoad) {
  auto handle = createSceneAsset();

  {
    auto entity = scene.entityDatabase.entity();
    entity.set<quoll::Id>({3});
    entity.set<quoll::PerspectiveLens>({});

    quoll::detail::EntitySerializer serializer(assetRegistry,
                                               scene.entityDatabase);

    auto entityNode = serializer.serialize(entity);

    createSceneFileWithEntity({entityNode.getData()});
  }

  sceneIO.loadScene(handle);

  auto activeCamera = scene.activeCamera;
  EXPECT_TRUE(activeCamera.is_valid());
  EXPECT_TRUE(activeCamera.has<quoll::PerspectiveLens>());
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

  auto environment = scene.activeEnvironment;
  EXPECT_TRUE(environment.is_valid());
  EXPECT_FALSE(environment.has<quoll::EnvironmentSkybox>());
  EXPECT_FALSE(environment.has<quoll::EnvironmentLightingSkyboxSource>());
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

    auto environment = scene.activeEnvironment;
    EXPECT_TRUE(environment.is_valid());
    EXPECT_FALSE(environment.has<quoll::EnvironmentSkybox>());
    EXPECT_FALSE(environment.has<quoll::EnvironmentLightingSkyboxSource>());
  }
}

TEST_F(
    SceneIOTest,
    CreatesEmptyEnvironmentEntityOnLoadIfEnvironmentDoesNotPointToValidEntity) {
  auto handle = createSceneAsset();
  auto zoneNode = getSceneYaml(handle)["zones"][0];
  zoneNode["environment"] = 100;

  sceneIO.loadScene(handle);

  auto environment = scene.activeEnvironment;
  EXPECT_TRUE(scene.activeEnvironment.is_valid());
  EXPECT_FALSE(environment.has<quoll::EnvironmentSkybox>());
  EXPECT_FALSE(environment.has<quoll::EnvironmentLightingSkyboxSource>());
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

  auto environment = scene.activeEnvironment;

  EXPECT_TRUE(environment.is_valid());
  EXPECT_EQ(environment.get_ref<quoll::Id>()->id, 125);
  EXPECT_FALSE(environment.has<quoll::EnvironmentSkybox>());
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

  auto environment = scene.activeEnvironment;

  EXPECT_TRUE(environment.is_valid());
  EXPECT_EQ(environment.get_ref<quoll::Id>()->id, 125);
  EXPECT_FALSE(environment.has<quoll::EnvironmentLightingSkyboxSource>());
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

  auto environment = scene.activeEnvironment;

  EXPECT_TRUE(environment.is_valid());
  EXPECT_EQ(environment.get_ref<quoll::Id>()->id, 125);
  EXPECT_TRUE(environment.has<quoll::EnvironmentLightingSkyboxSource>());
}
