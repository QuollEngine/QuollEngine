#include "liquid/core/Base.h"
#include "liquid-tests/Testing.h"

#include "liquid/entity/EntityDatabase.h"
#include "liquid/scene/SceneIO.h"

class SceneIOTest : public ::testing::Test {
public:
  static const liquid::Path SceneDirectory;
  static const liquid::Path SceneEntitiesDirectory;
  static const liquid::Path ScenePath;

public:
  SceneIOTest() : sceneIO(assetRegistry, scene) {}

  void SetUp() override {
    std::filesystem::create_directory(SceneDirectory);
    std::filesystem::create_directory(SceneEntitiesDirectory);
    std::ofstream stream(ScenePath);
    YAML::Node root;
    root["name"] = "TestScene";

    YAML::Node zoneNode;
    zoneNode["name"] = "TestZone";
    zoneNode["entities"] = "entities";
    zoneNode["startingCamera"] = 3;
    root["zones"][0] = zoneNode;
    root["persistentZone"] = 0;

    stream << root;
    stream.close();
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

  void TearDown() override { std::filesystem::remove_all(SceneDirectory); }

  liquid::String getEntityFile(liquid::Entity entity) {
    auto id = scene.entityDatabase.get<liquid::Id>(entity).id;
    return std::to_string(id) + ".lqnode";
  }

  void writeNodeToFile(const YAML::Node &node, const liquid::String &path) {
    std::ofstream stream(SceneEntitiesDirectory / (path + ".lqnode"));
    stream << node;
    stream.close();
  }

  void createSimpleEntityFile(uint64_t id, uint64_t parent = 0) {
    YAML::Node node;
    node["id"] = id;
    node["version"] = "0.1";

    if (parent > 0) {
      node["components"]["transform"]["parent"] = parent;
    }

    writeNodeToFile(node, std::to_string(id));
  }

public:
  liquid::AssetRegistry assetRegistry;
  liquid::Scene scene;
  liquid::SceneIO sceneIO;
};

const liquid::Path SceneIOTest::SceneDirectory =
    std::filesystem::current_path() / "scene-io-test";

const liquid::Path SceneIOTest::ScenePath =
    SceneIOTest::SceneDirectory / "main.lqscene";

const liquid::Path SceneIOTest::SceneEntitiesDirectory =
    SceneIOTest::SceneDirectory / "entities";

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfNodeDoesNotHaveId) {
  YAML::Node node;
  node["version"] = "0.1";

  writeNodeToFile(node, "test");
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
    node["version"] = "0.1";
    node["id"] = invalidNode;
    writeNodeToFile(node, "test");
    sceneIO.loadScene(ScenePath);

    EXPECT_EQ(scene.entityDatabase.getEntityCount(), 2);
  }
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdIsZero) {
  YAML::Node node;
  node["version"] = "0.1";
  node["id"] = 0;
  writeNodeToFile(node, "test");
  sceneIO.loadScene(ScenePath);

  EXPECT_EQ(scene.entityDatabase.getEntityCount(), 2);
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdIsNegative) {
  YAML::Node node;
  node["version"] = "0.1";
  node["id"] = -1;
  writeNodeToFile(node, "test");
  sceneIO.loadScene(ScenePath);

  EXPECT_EQ(scene.entityDatabase.getEntityCount(), 2);
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdAlreadyExists) {
  YAML::Node node;
  node["version"] = "0.1";
  node["id"] = 50;

  writeNodeToFile(node, "test");
  sceneIO.loadScene(ScenePath);

  // first one is added
  EXPECT_EQ(scene.entityDatabase.getEntityCount(), 3);

  sceneIO.loadScene(ScenePath);

  // second addition will fail due to duplicate Id
  EXPECT_EQ(scene.entityDatabase.getEntityCount(), 3);
}

TEST_F(SceneIOTest, LoadingSceneFilesFromDirectoryCreatesOneEntityPerFile) {
  static constexpr uint64_t NumEntities = 9;

  for (uint64_t i = 1; i < NumEntities + 1; ++i) {
    createSimpleEntityFile(i);
  }

  const auto &entities = sceneIO.loadScene(ScenePath);

  EXPECT_GT(scene.entityDatabase.getEntityCount(), entities.size() + 1);
  for (auto entity : entities) {
    EXPECT_TRUE(scene.entityDatabase.has<liquid::Id>(entity));
  }
}

TEST_F(SceneIOTest, SavingSceneAfterLoadingCreatesEntityWithNonConflictingId) {
  static constexpr uint64_t NumEntities = 9;

  for (uint64_t i = 1; i < NumEntities + 1; ++i) {
    createSimpleEntityFile(i);
  }

  sceneIO.loadScene(ScenePath);

  auto entity = scene.entityDatabase.create();

  sceneIO.saveEntity(entity, ScenePath);

  auto id = scene.entityDatabase.get<liquid::Id>(entity).id;

  EXPECT_EQ(id, 10);
  EXPECT_TRUE(
      std::filesystem::is_regular_file(SceneEntitiesDirectory / "10.lqnode"));
}

TEST_F(SceneIOTest, LoadingSetsParentsProperly) {
  static constexpr uint64_t NumEntities = 9;

  for (uint64_t i = 1; i < NumEntities + 1; ++i) {
    // set parent to next entity
    // to make sure that parent entities
    // are loaded after child ones
    createSimpleEntityFile(i, i + 1);
  }

  const auto &entities = sceneIO.loadScene(ScenePath);

  EXPECT_GT(scene.entityDatabase.getEntityCount(), entities.size() + 1);
  EXPECT_EQ(scene.entityDatabase.getEntityCountForComponent<liquid::Parent>(),
            entities.size() - 1);
}

TEST_F(SceneIOTest, LoadingSetsParentsFromPreviousSaveProperly) {
  static constexpr uint64_t NumEntities = 9;

  auto parent = scene.entityDatabase.create();

  // Creates ID for the parent and store it in cache
  sceneIO.saveEntity(parent, ScenePath);

  auto parentId = scene.entityDatabase.get<liquid::Id>(parent).id;

  createSimpleEntityFile(10, parentId);

  sceneIO.loadScene(ScenePath);

  auto lastAddedEntity =
      static_cast<liquid::Entity>(scene.entityDatabase.getEntityCount());

  EXPECT_TRUE(scene.entityDatabase.has<liquid::Parent>(lastAddedEntity));
  EXPECT_EQ(scene.entityDatabase.get<liquid::Parent>(lastAddedEntity).parent,
            parent);
}

TEST_F(SceneIOTest, SavingEntityCreatesIdComponentIfComponentDoesNotExist) {
  auto entity = scene.entityDatabase.create();
  sceneIO.saveEntity(entity, ScenePath);

  EXPECT_TRUE(scene.entityDatabase.has<liquid::Id>(entity));
}

TEST_F(SceneIOTest, SavingNewEntityCreatesEntityFile) {
  auto entity = scene.entityDatabase.create();

  sceneIO.saveEntity(entity, ScenePath);

  auto path = SceneEntitiesDirectory / getEntityFile(entity);
  EXPECT_TRUE(std::filesystem::is_regular_file(path));
}

TEST_F(SceneIOTest, SavingExistingEntityCreatesEntityFile) {
  auto entity = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::Id>(entity, {155});
  sceneIO.saveEntity(entity, ScenePath);

  auto path = SceneEntitiesDirectory / "155.lqnode";
  EXPECT_TRUE(std::filesystem::is_regular_file(path));
}

TEST_F(SceneIOTest, CreatesDummyCameraComponentOnConstruct) {
  EXPECT_TRUE(scene.entityDatabase.exists(scene.dummyCamera));
  EXPECT_TRUE(scene.entityDatabase.has<liquid::Camera>(scene.dummyCamera));
}

TEST_F(SceneIOTest, SetsInitialCameraAsTheActiveCameraOnLoad) {
  {
    auto entity = scene.entityDatabase.create();
    scene.entityDatabase.set<liquid::Id>(entity, {15});
    scene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});

    sceneIO.saveStartingCamera(entity, ScenePath);
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

  sceneIO.saveStartingCamera(entity, ScenePath);
  sceneIO.deleteEntityFilesAndRelations(entity, ScenePath);

  EXPECT_EQ(scene.activeCamera, another);
}

TEST_F(SceneIOTest,
       DeletingStartingCameraSetsDummyCameraAsTheCameraIfNoOtherCameraExists) {
  auto entity = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::Id>(entity, {15});
  scene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});

  sceneIO.saveStartingCamera(entity, ScenePath);
  sceneIO.deleteEntityFilesAndRelations(entity, ScenePath);

  EXPECT_EQ(scene.activeCamera, scene.dummyCamera);
}

TEST_F(SceneIOTest, SavesEntityAsInitialCameraIfItHasCameraComponent) {
  auto entity = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::Id>(entity, {15});
  scene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});

  sceneIO.saveEntity(entity, ScenePath);
  sceneIO.saveStartingCamera(entity, ScenePath);

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
  scene.entityDatabase.set<liquid::Id>(entity, {15});

  sceneIO.saveEntity(entity, ScenePath);
  sceneIO.saveStartingCamera(entity, ScenePath);

  sceneIO.reset();

  sceneIO.loadScene(ScenePath);

  EXPECT_EQ(scene.activeCamera, scene.dummyCamera);
}

TEST_F(SceneIOTest,
       DoesNotSaveEntityAsInitialCameraIfItDoesNotHaveIdComponent) {
  auto entity = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  sceneIO.saveStartingCamera(entity, ScenePath);
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
  asset.name = "test-env";
  asset.relativePath = liquid::Path("my-dir") / "test-env.hdr";
  auto handle = assetRegistry.getEnvironments().addAsset(asset);

  auto node = loadSceneFile(ScenePath);
  auto zoneNode = node["zones"][0];
  zoneNode["environment"]["skybox"]["type"] = "texture";
  zoneNode["environment"]["skybox"]["texture"] = "my-dir/test-env.hdr";
  saveSceneFile(node, ScenePath);

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.environment));
  EXPECT_TRUE(
      scene.entityDatabase.has<liquid::EnvironmentSkybox>(scene.environment));
  EXPECT_EQ(
      scene.entityDatabase.get<liquid::EnvironmentSkybox>(scene.environment)
          .environmentHandle,
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
  asset.name = "test-env";
  asset.relativePath = liquid::Path("my-dir") / "test-env.hdr";
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
  scene.environment = liquid::EntityNull;
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

TEST_F(SceneIOTest, SetsSkyboxToNoneOnSaveIfNoSkyboxComponent) {
  liquid::AssetData<liquid::EnvironmentAsset> asset{};
  asset.name = "test-env";
  asset.relativePath = liquid::Path("my-dir") / "test-env.hdr";

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

TEST_F(
    SceneIOTest,
    SetsSkyboxTypeToTextureOnSaveIfSceneEnvironmentEntityHasSkyboxTextureComponent) {
  liquid::AssetData<liquid::EnvironmentAsset> asset{};
  asset.name = "test-env";
  asset.relativePath = liquid::Path("my-dir") / "test-env.hdr";

  auto handle = assetRegistry.getEnvironments().addAsset(asset);

  scene.environment = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::EnvironmentSkybox>(scene.environment,
                                                      {handle});

  sceneIO.saveEnvironment(ScenePath);

  auto node = loadSceneFile(ScenePath);
  auto envNode = node["zones"][0]["environment"];

  EXPECT_TRUE(envNode.IsMap());
  EXPECT_TRUE(envNode["skybox"].IsMap());
  EXPECT_TRUE(envNode["skybox"]["type"].IsScalar());
  EXPECT_EQ(envNode["skybox"]["type"].as<liquid::String>(), "texture");
  EXPECT_EQ(envNode["skybox"]["texture"].as<liquid::String>(),
            "my-dir/test-env.hdr");
}

TEST_F(
    SceneIOTest,
    SetsEnvironmentLightingToNullOnSaveIfNoEnvironmentLightingSourceComponent) {
  liquid::AssetData<liquid::EnvironmentAsset> asset{};
  asset.name = "test-env";
  asset.relativePath = liquid::Path("my-dir") / "test-env.hdr";
  auto handle = assetRegistry.getEnvironments().addAsset(asset);

  scene.environment = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::EnvironmentSkybox>(scene.environment,
                                                      {handle});

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
  asset.name = "test-env";
  asset.relativePath = liquid::Path("my-dir") / "test-env.hdr";

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
