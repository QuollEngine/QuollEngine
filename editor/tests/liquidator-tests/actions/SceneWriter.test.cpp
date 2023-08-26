#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/yaml/Yaml.h"

#include "liquidator/asset/SceneWriter.h"

#include "liquidator-tests/Testing.h"

static liquid::Path TmpPath = std::filesystem::current_path() / "tmp";
static liquid::Path ScenePath = TmpPath / "main.scene";

class SceneWriterTest : public ::testing::Test {
public:
  SceneWriterTest() : writer(scene, assetRegistry) {}

  void SetUp() {
    TearDown();
    std::filesystem::create_directories(TmpPath);
    std::filesystem::copy_file(std::filesystem::current_path() / "fixtures" /
                                   "valid-asset.scene",
                               ScenePath);

    writer.open(ScenePath);
  }

  void TearDown() { std::filesystem::remove_all(TmpPath); }

  YAML::Node loadSceneFile() {
    std::ifstream stream(ScenePath);
    auto node = YAML::Load(stream);
    stream.close();

    return node;
  }

  liquid::AssetRegistry assetRegistry;
  liquid::Scene scene;
  liquid::editor::SceneWriter writer;
};

TEST_F(SceneWriterTest, SavingEntityCreatesIdComponentForEntityThatHasNoId) {
  auto entity = scene.entityDatabase.create();
  writer.saveEntities({entity});

  EXPECT_TRUE(scene.entityDatabase.has<liquid::Id>(entity));
}

TEST_F(SceneWriterTest, SavingEntityWithIdDoesNotRegenerateTheId) {
  auto entity = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::Id>(entity, {10});
  writer.saveEntities({entity});

  EXPECT_TRUE(scene.entityDatabase.has<liquid::Id>(entity));
  EXPECT_EQ(scene.entityDatabase.get<liquid::Id>(entity).id, 10);
}

TEST_F(SceneWriterTest, SavingEntityCreatesNonconflictingId) {
  auto e1 = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::Id>(e1, {10});

  writer.open(ScenePath);

  auto e2 = scene.entityDatabase.create();

  writer.saveEntities({e2});

  EXPECT_TRUE(scene.entityDatabase.has<liquid::Id>(e2));
  EXPECT_NE(scene.entityDatabase.get<liquid::Id>(e2).id, 10);
}

TEST_F(SceneWriterTest, SavingNewEntityAddsNewNodeInSceneFile) {
  auto e1 = scene.entityDatabase.create();
  auto e2 = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::Id>(e1, {155});
  scene.entityDatabase.set<liquid::Name>(e1, {"E1"});
  scene.entityDatabase.set<liquid::Name>(e2, {"E2"});

  writer.saveEntities({e1, e2});

  auto node = loadSceneFile();
  EXPECT_EQ(node["entities"][1]["id"].as<uint64_t>(0),
            scene.entityDatabase.get<liquid::Id>(e2).id);
  EXPECT_EQ(node["entities"][1]["name"].as<liquid::String>(""), "E2");
}

TEST_F(SceneWriterTest, SavingExistingEntityUpdatesExistingNodeInSceneFile) {
  auto e1 = scene.entityDatabase.create();
  auto e2 = scene.entityDatabase.create();
  {
    scene.entityDatabase.set<liquid::Id>(e1, {155});
    scene.entityDatabase.set<liquid::Name>(e1, {"E1"});
    scene.entityDatabase.set<liquid::Name>(e2, {"E2"});

    writer.saveEntities({e1, e2});

    auto node = loadSceneFile();
    EXPECT_EQ(node["entities"].size(), 2);
    EXPECT_EQ(node["entities"][0]["id"].as<uint64_t>(0), 155);
    EXPECT_EQ(node["entities"][0]["name"].as<liquid::String>(""), "E1");

    EXPECT_EQ(node["entities"][1]["id"].as<uint64_t>(0),
              scene.entityDatabase.get<liquid::Id>(e2).id);
    EXPECT_EQ(node["entities"][1]["name"].as<liquid::String>(""), "E2");
  }

  {
    scene.entityDatabase.set<liquid::Name>(e1, {"E1 New"});

    writer.saveEntities({e1});

    auto node = loadSceneFile();
    EXPECT_EQ(node["entities"].size(), 2);
    EXPECT_EQ(node["entities"][0]["id"].as<uint32_t>(0), 155);
    EXPECT_EQ(node["entities"][0]["name"].as<liquid::String>(""), "E1 New");

    EXPECT_EQ(node["entities"][1]["id"].as<uint64_t>(0),
              scene.entityDatabase.get<liquid::Id>(e2).id);
    EXPECT_EQ(node["entities"][1]["name"].as<liquid::String>(""), "E2");
  }
}

TEST_F(SceneWriterTest, SavingEntitySavesParentBeforeEntityIfParentIsNotSaved) {
  auto entity = scene.entityDatabase.create();
  auto parent = scene.entityDatabase.create();
  auto parent2 = scene.entityDatabase.create();

  scene.entityDatabase.set<liquid::Parent>(entity, {parent});
  scene.entityDatabase.set<liquid::Parent>(parent, {parent2});

  writer.saveEntities({entity});

  ASSERT_TRUE(scene.entityDatabase.has<liquid::Id>(entity));
  ASSERT_TRUE(scene.entityDatabase.has<liquid::Id>(parent));
  ASSERT_TRUE(scene.entityDatabase.has<liquid::Id>(parent2));

  auto entityId = scene.entityDatabase.get<liquid::Id>(entity).id;
  auto parentId = scene.entityDatabase.get<liquid::Id>(parent).id;
  auto parent2Id = scene.entityDatabase.get<liquid::Id>(parent2).id;

  auto node = loadSceneFile();
  EXPECT_EQ(node["entities"].size(), 3);
  EXPECT_EQ(node["entities"][0]["id"].as<uint64_t>(0), parent2Id);
  EXPECT_EQ(node["entities"][1]["id"].as<uint64_t>(0), parentId);
  EXPECT_EQ(node["entities"][2]["id"].as<uint64_t>(0), entityId);
}

TEST_F(SceneWriterTest, SavingEntityAndParentTogetherSavesTheParentOnce) {
  auto entity = scene.entityDatabase.create();
  auto parent = scene.entityDatabase.create();
  auto parent2 = scene.entityDatabase.create();

  scene.entityDatabase.set<liquid::Parent>(entity, {parent});
  scene.entityDatabase.set<liquid::Parent>(parent, {parent2});

  writer.saveEntities({entity, parent});

  ASSERT_TRUE(scene.entityDatabase.has<liquid::Id>(entity));
  ASSERT_TRUE(scene.entityDatabase.has<liquid::Id>(parent));
  ASSERT_TRUE(scene.entityDatabase.has<liquid::Id>(parent2));

  auto entityId = scene.entityDatabase.get<liquid::Id>(entity).id;
  auto parentId = scene.entityDatabase.get<liquid::Id>(parent).id;
  auto parent2Id = scene.entityDatabase.get<liquid::Id>(parent2).id;

  auto node = loadSceneFile();
  EXPECT_EQ(node["entities"].size(), 3);
  EXPECT_EQ(node["entities"][0]["id"].as<uint64_t>(0), parent2Id);
  EXPECT_EQ(node["entities"][1]["id"].as<uint64_t>(0), parentId);
  EXPECT_EQ(node["entities"][2]["id"].as<uint64_t>(0), entityId);
}

TEST_F(SceneWriterTest, DeletingEntityDeletesItFromSceneFile) {
  auto e1 = scene.entityDatabase.create();
  auto e2 = scene.entityDatabase.create();
  auto e3 = scene.entityDatabase.create();

  {
    writer.saveEntities({e1, e2, e3});

    auto node = loadSceneFile();
    EXPECT_EQ(node["entities"].size(), 3);
    EXPECT_EQ(node["entities"][0]["id"].as<uint64_t>(1),
              scene.entityDatabase.get<liquid::Id>(e1).id);

    EXPECT_EQ(node["entities"][1]["id"].as<uint64_t>(2),
              scene.entityDatabase.get<liquid::Id>(e2).id);

    EXPECT_EQ(node["entities"][2]["id"].as<uint64_t>(3),
              scene.entityDatabase.get<liquid::Id>(e3).id);
  }

  {
    writer.deleteEntities({e1, e3});

    auto node = loadSceneFile();
    EXPECT_EQ(node["entities"].size(), 1);
    EXPECT_EQ(node["entities"][0]["id"].as<uint64_t>(0),
              scene.entityDatabase.get<liquid::Id>(e2).id);
  }
}

TEST_F(SceneWriterTest, DeletingEntityDeletesItsChildrenFromSceneFile) {
  auto e1 = scene.entityDatabase.create();
  auto e2 = scene.entityDatabase.create();
  auto e3 = scene.entityDatabase.create();
  auto e4 = scene.entityDatabase.create();

  {
    scene.entityDatabase.set<liquid::Parent>(e2, {e1});

    scene.entityDatabase.set<liquid::Parent>(e3, {e1});

    scene.entityDatabase.set<liquid::Parent>(e4, {e2});
    writer.saveEntities({e1, e2, e3, e4});

    scene.entityDatabase.set<liquid::Children>(e1, {{e2, e3}});
    scene.entityDatabase.set<liquid::Children>(e3, {{e4}});

    auto node = loadSceneFile();
    EXPECT_EQ(node["entities"].size(), 4);
  }

  {
    writer.deleteEntities({e1});

    std::ifstream stream(ScenePath);
    auto node = YAML::Load(stream);
    stream.close();

    EXPECT_EQ(node["entities"].size(), 0);
  }
}

TEST_F(SceneWriterTest, DeletingStartingCameraSetsStartingCameraToNull) {
  auto e1 = scene.entityDatabase.create();
  scene.activeCamera = e1;
  scene.entityDatabase.set<liquid::PerspectiveLens>(e1, {});

  writer.saveEntities({e1});
  writer.saveScene();

  {
    auto node = loadSceneFile();
    EXPECT_EQ(node["zones"][0]["startingCamera"].as<uint64_t>(0),
              scene.entityDatabase.get<liquid::Id>(e1).id);
  }

  {
    writer.deleteEntities({e1});
    auto node = loadSceneFile();
    EXPECT_TRUE(node["zones"][0]["startingCamera"].IsNull());
  }
}

TEST_F(SceneWriterTest, DeletingEnvironmentSetsEnvironmentToNull) {
  auto e1 = scene.entityDatabase.create();
  scene.activeEnvironment = e1;

  writer.saveEntities({e1});
  writer.saveScene();

  {
    auto node = loadSceneFile();
    EXPECT_EQ(node["zones"][0]["environment"].as<uint64_t>(0),
              scene.entityDatabase.get<liquid::Id>(e1).id);
  }

  {
    writer.deleteEntities({e1});
    auto node = loadSceneFile();
    EXPECT_TRUE(node["zones"][0]["environment"].IsNull());
  }
}

TEST_F(SceneWriterTest,
       DoesNotSaveEntityAsInitialCameraIfItDoesNotHaveCameraComponent) {
  auto entity = scene.entityDatabase.create();
  writer.saveEntities({entity});

  scene.activeCamera = entity;
  writer.saveScene();

  auto node = loadSceneFile();
  EXPECT_FALSE(node["zones"][0]["startingCamera"]);
}

TEST_F(SceneWriterTest,
       DoesNotSaveEntityAsInitialCameraIfItDoesNotHaveIdComponent) {
  auto entity = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});

  scene.activeCamera = entity;
  writer.saveScene();

  auto node = loadSceneFile();
  EXPECT_FALSE(node["zones"][0]["startingCamera"]);
}

TEST_F(SceneWriterTest, SavesEntityAsStartingCameraIfItHasCameraComponent) {
  auto entity = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::Id>(entity, {15});
  scene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  writer.saveEntities({entity});

  scene.activeCamera = entity;
  writer.saveScene();

  auto node = loadSceneFile();
  EXPECT_EQ(node["zones"][0]["startingCamera"].as<uint64_t>(0), 15);
}

TEST_F(SceneWriterTest, DoesNotSaveEntityAsEnvironmentIfItDoesNotHaveId) {
  scene.activeEnvironment = scene.entityDatabase.create();
  writer.saveScene();

  auto node = loadSceneFile();
  EXPECT_FALSE(node["zones"][0]["environment"].IsNull());
}

TEST_F(SceneWriterTest, SetsEnvironmentToEntityIdIfEnvironmentEntityHasId) {
  scene.activeEnvironment = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::Id>(scene.activeEnvironment, {10});

  writer.saveScene();

  auto node = loadSceneFile();
  auto envNode = node["zones"][0]["environment"];

  EXPECT_EQ(envNode.as<uint64_t>(0), 10);
}
