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

  void TearDown() override { std::filesystem::remove_all(SceneDirectory); }

  liquid::String getEntityFile(liquid::Entity entity) {
    auto id = scene.entityDatabase.get<liquid::IdComponent>(entity).id;
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

  EXPECT_EQ(scene.entityDatabase.getEntityCount(), 1);
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

    EXPECT_EQ(scene.entityDatabase.getEntityCount(), 1);
  }
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdIsZero) {
  YAML::Node node;
  node["version"] = "0.1";
  node["id"] = 0;
  writeNodeToFile(node, "test");
  sceneIO.loadScene(ScenePath);

  EXPECT_EQ(scene.entityDatabase.getEntityCount(), 1);
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdIsNegative) {
  YAML::Node node;
  node["version"] = "0.1";
  node["id"] = -1;
  writeNodeToFile(node, "test");
  sceneIO.loadScene(ScenePath);

  EXPECT_EQ(scene.entityDatabase.getEntityCount(), 1);
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdAlreadyExists) {
  YAML::Node node;
  node["version"] = "0.1";
  node["id"] = 50;

  writeNodeToFile(node, "test");
  sceneIO.loadScene(ScenePath);

  // first one is added
  EXPECT_EQ(scene.entityDatabase.getEntityCount(), 2);

  sceneIO.loadScene(ScenePath);

  // second addition will fail due to duplicate Id
  EXPECT_EQ(scene.entityDatabase.getEntityCount(), 2);
}

TEST_F(SceneIOTest, LoadingSceneFilesFromDirectoryCreatesOneEntityPerFile) {
  static constexpr uint64_t NumEntities = 9;

  for (uint64_t i = 1; i < NumEntities + 1; ++i) {
    createSimpleEntityFile(i);
  }

  const auto &entities = sceneIO.loadScene(ScenePath);

  EXPECT_EQ(scene.entityDatabase.getEntityCount(), entities.size() + 1);
  for (auto entity : entities) {
    EXPECT_TRUE(scene.entityDatabase.has<liquid::IdComponent>(entity));
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

  auto id = scene.entityDatabase.get<liquid::IdComponent>(entity).id;

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

  EXPECT_EQ(scene.entityDatabase.getEntityCount(), entities.size() + 1);
  EXPECT_EQ(scene.entityDatabase
                .getEntityCountForComponent<liquid::ParentComponent>(),
            entities.size() - 1);
}

TEST_F(SceneIOTest, LoadingSetsParentsFromPreviousSaveProperly) {
  static constexpr uint64_t NumEntities = 9;

  auto parent = scene.entityDatabase.create();

  // Creates ID for the parent and store it in cache
  sceneIO.saveEntity(parent, ScenePath);

  auto parentId = scene.entityDatabase.get<liquid::IdComponent>(parent).id;

  createSimpleEntityFile(10, parentId);

  sceneIO.loadScene(ScenePath);

  auto lastAddedEntity =
      static_cast<liquid::Entity>(scene.entityDatabase.getEntityCount());

  EXPECT_TRUE(
      scene.entityDatabase.has<liquid::ParentComponent>(lastAddedEntity));
  EXPECT_EQ(
      scene.entityDatabase.get<liquid::ParentComponent>(lastAddedEntity).parent,
      parent);
}

TEST_F(SceneIOTest, SavingEntityCreatesIdComponentIfComponentDoesNotExist) {
  auto entity = scene.entityDatabase.create();
  sceneIO.saveEntity(entity, ScenePath);

  EXPECT_TRUE(scene.entityDatabase.has<liquid::IdComponent>(entity));
}

TEST_F(SceneIOTest, SavingNewEntityCreatesEntityFile) {
  auto entity = scene.entityDatabase.create();

  sceneIO.saveEntity(entity, ScenePath);

  auto path = SceneEntitiesDirectory / getEntityFile(entity);
  EXPECT_TRUE(std::filesystem::is_regular_file(path));
}

TEST_F(SceneIOTest, SavingExistingEntityCreatesEntityFile) {
  auto entity = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::IdComponent>(entity, {155});
  sceneIO.saveEntity(entity, ScenePath);

  auto path = SceneEntitiesDirectory / "155.lqnode";
  EXPECT_TRUE(std::filesystem::is_regular_file(path));
}

TEST_F(SceneIOTest, CreatesDummyCameraComponentOnConstruct) {
  EXPECT_TRUE(scene.entityDatabase.exists(scene.dummyCamera));
  EXPECT_TRUE(
      scene.entityDatabase.has<liquid::CameraComponent>(scene.dummyCamera));
}

TEST_F(SceneIOTest, SetsInitialCameraAsTheActiveCameraOnLoad) {
  {
    auto entity = scene.entityDatabase.create();
    scene.entityDatabase.set<liquid::IdComponent>(entity, {15});
    scene.entityDatabase.set<liquid::PerspectiveLensComponent>(entity, {});

    sceneIO.saveStartingCamera(entity, ScenePath);
  }

  sceneIO.loadScene(ScenePath);

  EXPECT_TRUE(scene.entityDatabase.exists(scene.activeCamera));
  EXPECT_TRUE(scene.entityDatabase.has<liquid::PerspectiveLensComponent>(
      scene.activeCamera));
}

TEST_F(SceneIOTest,
       SetsDummyCameraAsTheActiveCameraOnLoadIfNoCameraExistsForTheScene) {
  sceneIO.loadScene(ScenePath);
  EXPECT_EQ(scene.activeCamera, scene.dummyCamera);
}

TEST_F(SceneIOTest,
       DeletingStartingCameraSetsLastCameraAsTheNewStartingCameraIfExists) {
  auto entity = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::IdComponent>(entity, {15});
  scene.entityDatabase.set<liquid::PerspectiveLensComponent>(entity, {});

  auto another = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::IdComponent>(another, {25});
  scene.entityDatabase.set<liquid::PerspectiveLensComponent>(another, {});

  sceneIO.saveStartingCamera(entity, ScenePath);
  sceneIO.deleteEntityFilesAndRelations(entity, ScenePath);

  EXPECT_EQ(scene.activeCamera, another);
}

TEST_F(SceneIOTest,
       DeletingStartingCameraSetsDummyCameraAsTheCameraIfNoOtherCameraExists) {
  auto entity = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::IdComponent>(entity, {15});
  scene.entityDatabase.set<liquid::PerspectiveLensComponent>(entity, {});

  sceneIO.saveStartingCamera(entity, ScenePath);
  sceneIO.deleteEntityFilesAndRelations(entity, ScenePath);

  EXPECT_EQ(scene.activeCamera, scene.dummyCamera);
}

TEST_F(SceneIOTest, SavesEntityAsInitialCameraIfItHasCameraComponent) {
  auto entity = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::IdComponent>(entity, {15});
  scene.entityDatabase.set<liquid::PerspectiveLensComponent>(entity, {});

  sceneIO.saveEntity(entity, ScenePath);
  sceneIO.saveStartingCamera(entity, ScenePath);

  sceneIO.reset();

  sceneIO.loadScene(ScenePath);

  EXPECT_NE(scene.activeCamera, scene.dummyCamera);
  EXPECT_EQ(scene.entityDatabase.get<liquid::IdComponent>(entity).id, 15);
  EXPECT_TRUE(
      scene.entityDatabase.has<liquid::PerspectiveLensComponent>(entity));
}

TEST_F(SceneIOTest,
       DoesNotSaveEntityAsInitialCameraIfItDoesNotHaveCameraComponent) {
  auto entity = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::IdComponent>(entity, {15});

  sceneIO.saveEntity(entity, ScenePath);
  sceneIO.saveStartingCamera(entity, ScenePath);

  sceneIO.reset();

  sceneIO.loadScene(ScenePath);

  EXPECT_EQ(scene.activeCamera, scene.dummyCamera);
}

TEST_F(SceneIOTest,
       DoesNotSaveEntityAsInitialCameraIfItDoesNotHaveIdComponent) {
  auto entity = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::PerspectiveLensComponent>(entity, {});

  sceneIO.saveStartingCamera(entity, ScenePath);

  sceneIO.reset();

  sceneIO.loadScene(ScenePath);

  EXPECT_EQ(scene.activeCamera, scene.dummyCamera);
}
