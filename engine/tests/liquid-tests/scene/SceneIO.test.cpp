#include "liquid/core/Base.h"
#include "liquid-tests/Testing.h"

#include "liquid/entity/EntityDatabase.h"
#include "liquid/scene/SceneIO.h"

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

// TODO: Updater after migration
TEST_F(SceneIOTest, LoadingSetsParentsFromPreviousSaveProperly) {
  static constexpr uint64_t NumEntities = 9;

  auto parent = scene.entityDatabase.create();

  // Creates ID for the parent and store it in cache
  sceneIO.saveEntities({parent}, ScenePath);

  auto parentId = scene.entityDatabase.get<liquid::Id>(parent).id;

  {
    YAML::Node node;
    node["id"] = 200;
    node["transform"]["parent"] = parentId;
    createSceneFileWithEntity({node});
  }

  sceneIO.loadScene(ScenePath);

  auto lastAddedEntity =
      static_cast<liquid::Entity>(scene.entityDatabase.getEntityCount());

  EXPECT_TRUE(scene.entityDatabase.has<liquid::Parent>(lastAddedEntity));
  EXPECT_EQ(scene.entityDatabase.get<liquid::Parent>(lastAddedEntity).parent,
            parent);
}

TEST_F(SceneIOTest, SavingEntityAfterLoadingCreatesEntityWithNonConflictingId) {
  static constexpr uint64_t NumEntities = 9;

  std::vector<YAML::Node> nodes(NumEntities);

  for (uint64_t i = 1; i < NumEntities + 1; ++i) {
    YAML::Node node;
    node["id"] = i;
    nodes.push_back(node);
  }

  createSceneFileWithEntity(nodes);

  sceneIO.loadScene(ScenePath);

  auto entity = scene.entityDatabase.create();

  sceneIO.saveEntities({entity}, ScenePath);

  auto id = scene.entityDatabase.get<liquid::Id>(entity).id;
  EXPECT_EQ(id, 10);
}

TEST_F(SceneIOTest, SavingEntitySavesParentBeforeEntityIfParentHasNoId) {
  auto entity = scene.entityDatabase.create();
  auto parent = scene.entityDatabase.create();
  auto parent2 = scene.entityDatabase.create();

  scene.entityDatabase.set<liquid::Parent>(entity, {parent});
  scene.entityDatabase.set<liquid::Parent>(parent, {parent2});

  sceneIO.saveEntities({entity}, ScenePath);

  ASSERT_TRUE(scene.entityDatabase.has<liquid::Id>(entity));
  ASSERT_TRUE(scene.entityDatabase.has<liquid::Id>(parent));
  ASSERT_TRUE(scene.entityDatabase.has<liquid::Id>(parent2));

  auto entityId = scene.entityDatabase.get<liquid::Id>(entity).id;
  auto parentId = scene.entityDatabase.get<liquid::Id>(parent).id;
  auto parent2Id = scene.entityDatabase.get<liquid::Id>(parent2).id;

  std::ifstream stream(ScenePath);
  auto node = YAML::Load(stream);
  stream.close();

  EXPECT_EQ(node["entities"].size(), 3);
  EXPECT_EQ(node["entities"][0]["id"].as<uint64_t>(0), parent2Id);
  EXPECT_EQ(node["entities"][1]["id"].as<uint64_t>(0), parentId);
  EXPECT_EQ(node["entities"][2]["id"].as<uint64_t>(0), entityId);
}

TEST_F(SceneIOTest, SavingEntityAndParentTogetherOnlySavesTheParentOnce) {
  auto entity = scene.entityDatabase.create();
  auto parent = scene.entityDatabase.create();
  auto parent2 = scene.entityDatabase.create();

  scene.entityDatabase.set<liquid::Parent>(entity, {parent});
  scene.entityDatabase.set<liquid::Parent>(parent, {parent2});

  sceneIO.saveEntities({entity, parent}, ScenePath);

  ASSERT_TRUE(scene.entityDatabase.has<liquid::Id>(entity));
  ASSERT_TRUE(scene.entityDatabase.has<liquid::Id>(parent));
  ASSERT_TRUE(scene.entityDatabase.has<liquid::Id>(parent2));

  auto entityId = scene.entityDatabase.get<liquid::Id>(entity).id;
  auto parentId = scene.entityDatabase.get<liquid::Id>(parent).id;
  auto parent2Id = scene.entityDatabase.get<liquid::Id>(parent2).id;

  std::ifstream stream(ScenePath);
  auto node = YAML::Load(stream);
  stream.close();

  EXPECT_EQ(node["entities"].size(), 3);
  EXPECT_EQ(node["entities"][0]["id"].as<uint64_t>(0), parent2Id);
  EXPECT_EQ(node["entities"][1]["id"].as<uint64_t>(0), parentId);
  EXPECT_EQ(node["entities"][2]["id"].as<uint64_t>(0), entityId);
}

TEST_F(SceneIOTest, SavingEntityCreatesIdComponentIfComponentDoesNotExist) {
  auto entity = scene.entityDatabase.create();
  sceneIO.saveEntities({entity}, ScenePath);

  EXPECT_TRUE(scene.entityDatabase.has<liquid::Id>(entity));
}

TEST_F(SceneIOTest, SavingNewEntityAddsNewNodeInSceneFile) {
  auto e1 = scene.entityDatabase.create();
  auto e2 = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::Id>(e1, {155});
  scene.entityDatabase.set<liquid::Name>(e1, {"E1"});
  scene.entityDatabase.set<liquid::Name>(e2, {"E2"});

  sceneIO.saveEntities({e1, e2}, ScenePath);

  std::ifstream stream(ScenePath);
  auto node = YAML::Load(stream);
  stream.close();

  EXPECT_EQ(node["entities"][1]["id"].as<uint64_t>(0),
            scene.entityDatabase.get<liquid::Id>(e2).id);
  EXPECT_EQ(node["entities"][1]["name"].as<liquid::String>(""), "E2");
}

TEST_F(SceneIOTest, SavingExistingEntityUpdatesExistingNodeInSceneFile) {
  auto e1 = scene.entityDatabase.create();
  auto e2 = scene.entityDatabase.create();
  {
    scene.entityDatabase.set<liquid::Id>(e1, {155});
    scene.entityDatabase.set<liquid::Name>(e1, {"E1"});
    scene.entityDatabase.set<liquid::Name>(e2, {"E2"});

    sceneIO.saveEntities({e1, e2}, ScenePath);

    std::ifstream stream(ScenePath);
    auto node = YAML::Load(stream);
    stream.close();

    EXPECT_EQ(node["entities"].size(), 2);
    EXPECT_EQ(node["entities"][0]["id"].as<uint64_t>(0), 155);
    EXPECT_EQ(node["entities"][0]["name"].as<liquid::String>(""), "E1");

    EXPECT_EQ(node["entities"][1]["id"].as<uint64_t>(0),
              scene.entityDatabase.get<liquid::Id>(e2).id);
    EXPECT_EQ(node["entities"][1]["name"].as<liquid::String>(""), "E2");
  }

  {
    scene.entityDatabase.set<liquid::Name>(e1, {"E1 New"});

    sceneIO.saveEntities({e1}, ScenePath);
    std::ifstream stream(ScenePath);
    auto node = YAML::Load(stream);
    stream.close();

    EXPECT_EQ(node["entities"].size(), 2);
    EXPECT_EQ(node["entities"][0]["id"].as<uint32_t>(0), 155);
    EXPECT_EQ(node["entities"][0]["name"].as<liquid::String>(""), "E1 New");

    EXPECT_EQ(node["entities"][1]["id"].as<uint64_t>(0),
              scene.entityDatabase.get<liquid::Id>(e2).id);
    EXPECT_EQ(node["entities"][1]["name"].as<liquid::String>(""), "E2");
  }
}

TEST_F(SceneIOTest, DeletingEntityDeletesItFromSceneFile) {
  auto e1 = scene.entityDatabase.create();
  auto e2 = scene.entityDatabase.create();
  {

    sceneIO.saveEntities({e1, e2}, ScenePath);

    std::ifstream stream(ScenePath);
    auto node = YAML::Load(stream);
    stream.close();

    EXPECT_EQ(node["entities"].size(), 2);
    EXPECT_EQ(node["entities"][0]["id"].as<uint64_t>(0),
              scene.entityDatabase.get<liquid::Id>(e1).id);

    EXPECT_EQ(node["entities"][1]["id"].as<uint64_t>(0),
              scene.entityDatabase.get<liquid::Id>(e2).id);
  }

  {
    sceneIO.deleteEntities({e1}, ScenePath);

    std::ifstream stream(ScenePath);
    auto node = YAML::Load(stream);
    stream.close();

    EXPECT_EQ(node["entities"].size(), 1);
    EXPECT_EQ(node["entities"][0]["id"].as<uint64_t>(0),
              scene.entityDatabase.get<liquid::Id>(e2).id);
  }
}

TEST_F(SceneIOTest, DeletingEntityDeletesItsChildrenFromSceneFile) {
  auto e1 = scene.entityDatabase.create();
  auto e2 = scene.entityDatabase.create();
  auto e3 = scene.entityDatabase.create();
  auto e4 = scene.entityDatabase.create();

  {

    scene.entityDatabase.set<liquid::Parent>(e2, {e1});

    scene.entityDatabase.set<liquid::Parent>(e3, {e1});

    scene.entityDatabase.set<liquid::Parent>(e4, {e2});
    sceneIO.saveEntities({e1, e2, e3, e4}, ScenePath);

    scene.entityDatabase.set<liquid::Children>(e1, {{e2, e3}});
    scene.entityDatabase.set<liquid::Children>(e3, {{e4}});

    std::ifstream stream(ScenePath);
    auto node = YAML::Load(stream);
    stream.close();

    EXPECT_EQ(node["entities"].size(), 4);
  }

  {
    sceneIO.deleteEntities({e1}, ScenePath);

    std::ifstream stream(ScenePath);
    auto node = YAML::Load(stream);
    stream.close();

    EXPECT_EQ(node["entities"].size(), 0);
  }
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
    scene.entityDatabase.set<liquid::Id>(entity, {15});
    scene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});

    scene.activeCamera = entity;
    sceneIO.saveStartingCamera(ScenePath);
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
       DeletingStartingCameraSetsLastCameraAsTheNewStartingCameraIfExists) {
  auto entity = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::Id>(entity, {15});
  scene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});

  auto another = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::Id>(another, {25});
  scene.entityDatabase.set<liquid::PerspectiveLens>(another, {});

  scene.activeCamera = entity;
  sceneIO.saveStartingCamera(ScenePath);
  sceneIO.deleteEntities({entity}, ScenePath);

  EXPECT_EQ(scene.activeCamera, another);
}

TEST_F(SceneIOTest,
       DeletingStartingCameraSetsDummyCameraAsTheCameraIfNoOtherCameraExists) {
  auto entity = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::Id>(entity, {15});
  scene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});

  scene.activeCamera = entity;
  sceneIO.saveStartingCamera(ScenePath);
  sceneIO.deleteEntities({entity}, ScenePath);

  EXPECT_EQ(scene.activeCamera, scene.dummyCamera);
}

TEST_F(SceneIOTest, SavesEntityAsInitialCameraIfItHasCameraComponent) {
  auto entity = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::Id>(entity, {15});
  scene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  sceneIO.saveEntities({entity}, ScenePath);

  scene.activeCamera = entity;
  sceneIO.saveStartingCamera(ScenePath);

  sceneIO.reset();

  sceneIO.loadScene(ScenePath);

  EXPECT_NE(scene.activeCamera, scene.dummyCamera);
  for (auto [entity, id] : scene.entityDatabase.view<liquid::Id>()) {
    EXPECT_EQ(id.id, 15);
    EXPECT_TRUE(scene.entityDatabase.has<liquid::PerspectiveLens>(entity));
  }
}

TEST_F(SceneIOTest,
       DoesNotSaveEntityAsInitialCameraIfItDoesNotHaveCameraComponent) {
  auto entity = scene.entityDatabase.create();
  sceneIO.saveEntities({entity}, ScenePath);

  scene.activeCamera = entity;
  sceneIO.saveStartingCamera(ScenePath);

  sceneIO.reset();
  sceneIO.loadScene(ScenePath);

  EXPECT_EQ(scene.activeCamera, scene.dummyCamera);
}

TEST_F(SceneIOTest,
       DoesNotSaveEntityAsInitialCameraIfItDoesNotHaveIdComponent) {
  auto entity = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});

  scene.activeCamera = entity;
  sceneIO.saveStartingCamera(ScenePath);
  sceneIO.reset();

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

TEST_F(SceneIOTest, SetsEnvironmentToNullOnSaveIfSceneEnvironmentEntityIsNull) {
  scene.environment = liquid::Entity::Null;
  sceneIO.saveEnvironment(ScenePath);

  auto node = loadSceneFile(ScenePath);
  EXPECT_TRUE(node["zones"][0]["environment"].IsNull());
}

TEST_F(SceneIOTest,
       SetsEnvironmentToNullOnSaveIfNoSkyboxOrEnvironmentLightingComponents) {
  scene.environment = scene.entityDatabase.create();
  sceneIO.saveEnvironment(ScenePath);

  auto node = loadSceneFile(ScenePath);
  EXPECT_TRUE(node["zones"][0]["environment"].IsNull());
}

TEST_F(SceneIOTest, SetsSkyboxToNullOnSaveIfNoSkyboxComponent) {
  liquid::AssetData<liquid::EnvironmentAsset> asset{};
  asset.uuid = "test-env";

  auto handle = assetRegistry.getEnvironments().addAsset(asset);

  scene.environment = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::EnvironmentLightingSkyboxSource>(
      scene.environment, {});

  sceneIO.saveEnvironment(ScenePath);

  auto node = loadSceneFile(ScenePath);
  auto envNode = node["zones"][0]["environment"];

  EXPECT_TRUE(envNode.IsMap());
  EXPECT_TRUE(envNode["skybox"].IsNull());
}

TEST_F(SceneIOTest,
       SetsSkyboxToNullOnSaveIfSkyboxTypeIsTextureButTextureAssetDoesNotExist) {
  liquid::AssetData<liquid::EnvironmentAsset> asset{};
  asset.uuid = "test-env";

  auto handle = assetRegistry.getEnvironments().addAsset(asset);

  assetRegistry.getEnvironments().deleteAsset(handle);

  scene.environment = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::EnvironmentSkybox>(
      scene.environment, {liquid::EnvironmentSkyboxType::Texture, handle});
  scene.entityDatabase.set<liquid::EnvironmentLightingSkyboxSource>(
      scene.environment, {});

  sceneIO.saveEnvironment(ScenePath);

  auto node = loadSceneFile(ScenePath);
  auto envNode = node["zones"][0]["environment"];

  EXPECT_TRUE(envNode.IsMap());
  EXPECT_TRUE(envNode["skybox"].IsNull());
}

TEST_F(
    SceneIOTest,
    SetsSkyboxTypeToTextureOnSaveIfSceneEnvironmentSkyboxTypeIsTextureAndAssetExists) {
  liquid::AssetData<liquid::EnvironmentAsset> asset{};
  asset.uuid = "test-env";

  auto handle = assetRegistry.getEnvironments().addAsset(asset);

  scene.environment = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::EnvironmentSkybox>(
      scene.environment, {liquid::EnvironmentSkyboxType::Texture, handle});

  sceneIO.saveEnvironment(ScenePath);

  auto node = loadSceneFile(ScenePath);
  auto envNode = node["zones"][0]["environment"];

  EXPECT_TRUE(envNode.IsMap());
  EXPECT_TRUE(envNode["skybox"].IsMap());
  EXPECT_TRUE(envNode["skybox"]["type"].IsScalar());
  EXPECT_EQ(envNode["skybox"]["type"].as<liquid::String>(), "texture");
  EXPECT_EQ(envNode["skybox"]["texture"].as<liquid::String>(), "test-env");
  EXPECT_FALSE(envNode["skybox"]["color"]);
}

TEST_F(SceneIOTest,
       SetsSkyboxTypeToColorOnSaveIfSceneEnvironmentSkyboxTypeIsColor) {
  scene.environment = scene.entityDatabase.create();

  liquid::EnvironmentSkybox component{};
  component.type = liquid::EnvironmentSkyboxType::Color;
  component.color = glm::vec4(0.5f, 0.2f, 0.5f, 1.0f);
  scene.entityDatabase.set(scene.environment, component);

  sceneIO.saveEnvironment(ScenePath);

  auto node = loadSceneFile(ScenePath);
  auto envNode = node["zones"][0]["environment"];

  EXPECT_TRUE(envNode.IsMap());
  EXPECT_TRUE(envNode["skybox"].IsMap());
  EXPECT_TRUE(envNode["skybox"]["type"].IsScalar());
  EXPECT_EQ(envNode["skybox"]["type"].as<liquid::String>(), "color");
  EXPECT_FALSE(envNode["skybox"]["texture"]);
  EXPECT_EQ(envNode["skybox"]["color"].as<glm::vec4>(), component.color);
}

TEST_F(
    SceneIOTest,
    SetsEnvironmentLightingToNullOnSaveIfNoEnvironmentLightingSourceComponent) {
  liquid::AssetData<liquid::EnvironmentAsset> asset{};
  asset.uuid = "test-env";
  auto handle = assetRegistry.getEnvironments().addAsset(asset);

  scene.environment = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::EnvironmentSkybox>(
      scene.environment, {liquid::EnvironmentSkyboxType::Texture, handle});

  sceneIO.saveEnvironment(ScenePath);

  auto node = loadSceneFile(ScenePath);
  auto envNode = node["zones"][0]["environment"];

  EXPECT_TRUE(envNode.IsMap());
  EXPECT_TRUE(envNode["lighting"].IsNull());
}

TEST_F(
    SceneIOTest,
    SetsEnvironmentLightingSkyboxSourceComponentOnSaveIfLightingSourceIsSkybox) {
  liquid::AssetData<liquid::EnvironmentAsset> asset{};
  asset.uuid = "test-env";
  auto handle = assetRegistry.getEnvironments().addAsset(asset);

  scene.environment = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::EnvironmentLightingSkyboxSource>(
      scene.environment, {});

  sceneIO.saveEnvironment(ScenePath);

  auto node = loadSceneFile(ScenePath);
  auto envNode = node["zones"][0]["environment"];

  EXPECT_TRUE(envNode.IsMap());
  EXPECT_TRUE(envNode["lighting"].IsMap());
  EXPECT_TRUE(envNode["lighting"]["source"].IsScalar());
  EXPECT_EQ(envNode["lighting"]["source"].as<liquid::String>(), "skybox");
}
