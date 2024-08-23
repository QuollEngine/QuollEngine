#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/core/Id.h"
#include "quoll/core/Name.h"
#include "quoll/scene/Children.h"
#include "quoll/scene/Parent.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/yaml/Yaml.h"
#include "quoll/editor/scene/asset/SceneWriter.h"
#include "quoll/editor-tests/Testing.h"

static quoll::Path TmpPath = std::filesystem::current_path() / "tmp";
static quoll::Path ScenePath = TmpPath / "main.scene";

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

  quoll::AssetRegistry assetRegistry;
  quoll::Scene scene;
  quoll::editor::SceneWriter writer;
};

TEST_F(SceneWriterTest, SavingEntityCreatesIdComponentForEntityThatHasNoId) {
  auto entity = scene.entityDatabase.entity();
  writer.syncEntities({entity});

  EXPECT_TRUE(entity.has<quoll::Id>());
}

TEST_F(SceneWriterTest, SavingEntityWithIdDoesNotRegenerateTheId) {
  auto entity = scene.entityDatabase.entity();
  entity.set<quoll::Id>({10});
  writer.syncEntities({entity});

  EXPECT_TRUE(entity.has<quoll::Id>());
  EXPECT_EQ(entity.get_ref<quoll::Id>()->id, 10);
}

TEST_F(SceneWriterTest, SavingEntityCreatesNonconflictingId) {
  auto e1 = scene.entityDatabase.entity();
  e1.set<quoll::Id>({10});

  writer.open(ScenePath);

  auto e2 = scene.entityDatabase.entity();

  writer.syncEntities({e2});

  EXPECT_TRUE(e2.has<quoll::Id>());
  EXPECT_NE(e2.get_ref<quoll::Id>()->id, 10);
}

TEST_F(SceneWriterTest, SavingNewEntityAddsNewNodeInSceneFile) {
  auto e1 = scene.entityDatabase.entity();
  auto e2 = scene.entityDatabase.entity();
  e1.set<quoll::Id>({155});
  e1.set<quoll::Name>({"E1"});
  e2.set<quoll::Name>({"E2"});

  writer.syncEntities({e1, e2});

  auto node = loadSceneFile();
  EXPECT_EQ(node["entities"][1]["id"].as<u64>(0), e2.get_ref<quoll::Id>()->id);
  EXPECT_EQ(node["entities"][1]["name"].as<quoll::String>(""), "E2");
}

TEST_F(SceneWriterTest, SavingExistingEntityUpdatesExistingNodeInSceneFile) {
  auto e1 = scene.entityDatabase.entity();
  auto e2 = scene.entityDatabase.entity();
  {
    e1.set<quoll::Id>({155});
    e1.set<quoll::Name>({"E1"});
    e2.set<quoll::Name>({"E2"});

    writer.syncEntities({e1, e2});

    auto node = loadSceneFile();
    EXPECT_EQ(node["entities"].size(), 2);
    EXPECT_EQ(node["entities"][0]["id"].as<u64>(0), 155);
    EXPECT_EQ(node["entities"][0]["name"].as<quoll::String>(""), "E1");

    EXPECT_EQ(node["entities"][1]["id"].as<u64>(0),
              e2.get_ref<quoll::Id>()->id);
    EXPECT_EQ(node["entities"][1]["name"].as<quoll::String>(""), "E2");
  }

  {
    e1.set<quoll::Name>({"E1 New"});

    writer.syncEntities({e1});

    auto node = loadSceneFile();
    EXPECT_EQ(node["entities"].size(), 2);
    EXPECT_EQ(node["entities"][0]["id"].as<u32>(0), 155);
    EXPECT_EQ(node["entities"][0]["name"].as<quoll::String>(""), "E1 New");

    EXPECT_EQ(node["entities"][1]["id"].as<u64>(0),
              e2.get_ref<quoll::Id>()->id);
    EXPECT_EQ(node["entities"][1]["name"].as<quoll::String>(""), "E2");
  }
}

TEST_F(SceneWriterTest, SavingEntitySavesParentBeforeEntityIfParentIsNotSaved) {
  auto entity = scene.entityDatabase.entity();
  auto parent = scene.entityDatabase.entity();
  auto parent2 = scene.entityDatabase.entity();

  entity.set<quoll::Parent>({parent});
  parent.set<quoll::Parent>({parent2});

  writer.syncEntities({entity});

  ASSERT_TRUE(entity.has<quoll::Id>());
  ASSERT_TRUE(parent.has<quoll::Id>());
  ASSERT_TRUE(parent2.has<quoll::Id>());

  auto entityId = entity.get_ref<quoll::Id>()->id;
  auto parentId = parent.get_ref<quoll::Id>()->id;
  auto parent2Id = parent2.get_ref<quoll::Id>()->id;

  auto node = loadSceneFile();
  EXPECT_EQ(node["entities"].size(), 3);
  EXPECT_EQ(node["entities"][0]["id"].as<u64>(0), parent2Id);
  EXPECT_EQ(node["entities"][1]["id"].as<u64>(0), parentId);
  EXPECT_EQ(node["entities"][2]["id"].as<u64>(0), entityId);
}

TEST_F(SceneWriterTest, SavingEntityAndParentTogetherSavesTheParentOnce) {
  auto entity = scene.entityDatabase.entity();
  auto parent = scene.entityDatabase.entity();
  auto parent2 = scene.entityDatabase.entity();

  entity.set<quoll::Parent>({parent});
  parent.set<quoll::Parent>({parent2});

  writer.syncEntities({entity, parent});

  ASSERT_TRUE(entity.has<quoll::Id>());
  ASSERT_TRUE(parent.has<quoll::Id>());
  ASSERT_TRUE(parent2.has<quoll::Id>());

  auto entityId = entity.get_ref<quoll::Id>()->id;
  auto parentId = parent.get_ref<quoll::Id>()->id;
  auto parent2Id = parent2.get_ref<quoll::Id>()->id;

  auto node = loadSceneFile();
  EXPECT_EQ(node["entities"].size(), 3);
  EXPECT_EQ(node["entities"][0]["id"].as<u64>(0), parent2Id);
  EXPECT_EQ(node["entities"][1]["id"].as<u64>(0), parentId);
  EXPECT_EQ(node["entities"][2]["id"].as<u64>(0), entityId);
}

TEST_F(SceneWriterTest, DeletingEntityDeletesItFromSceneFile) {
  auto e1 = scene.entityDatabase.entity();
  auto e2 = scene.entityDatabase.entity();
  auto e3 = scene.entityDatabase.entity();

  {
    writer.syncEntities({e1, e2, e3});

    auto node = loadSceneFile();
    EXPECT_EQ(node["entities"].size(), 3);
    EXPECT_EQ(node["entities"][0]["id"].as<u64>(1),
              e1.get_ref<quoll::Id>()->id);

    EXPECT_EQ(node["entities"][1]["id"].as<u64>(2),
              e2.get_ref<quoll::Id>()->id);

    EXPECT_EQ(node["entities"][2]["id"].as<u64>(3),
              e3.get_ref<quoll::Id>()->id);
  }

  {
    writer.deleteEntities({e1, e3});

    auto node = loadSceneFile();
    EXPECT_EQ(node["entities"].size(), 1);
    EXPECT_EQ(node["entities"][0]["id"].as<u64>(0),
              e2.get_ref<quoll::Id>()->id);
  }
}

TEST_F(SceneWriterTest, DeletingEntityDeletesItsChildrenFromSceneFile) {
  auto e1 = scene.entityDatabase.entity();
  auto e2 = scene.entityDatabase.entity();
  auto e3 = scene.entityDatabase.entity();
  auto e4 = scene.entityDatabase.entity();

  {
    e2.set<quoll::Parent>({e1});

    e3.set<quoll::Parent>({e1});

    e4.set<quoll::Parent>({e2});
    writer.syncEntities({e1, e2, e3, e4});

    e1.set<quoll::Children>({{e2, e3}});
    e3.set<quoll::Children>({{e4}});

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
  auto e1 = scene.entityDatabase.entity();
  scene.activeCamera = e1;
  e1.set<quoll::PerspectiveLens>({});

  writer.syncEntities({e1});
  writer.syncScene();

  {
    auto node = loadSceneFile();
    EXPECT_EQ(node["zones"][0]["startingCamera"].as<u64>(0),
              e1.get_ref<quoll::Id>()->id);
  }

  {
    writer.deleteEntities({e1});
    auto node = loadSceneFile();
    EXPECT_TRUE(node["zones"][0]["startingCamera"].IsNull());
  }
}

TEST_F(SceneWriterTest, DeletingEnvironmentSetsEnvironmentToNull) {
  auto e1 = scene.entityDatabase.entity();
  scene.activeEnvironment = e1;

  writer.syncEntities({e1});
  writer.syncScene();

  {
    auto node = loadSceneFile();
    EXPECT_EQ(node["zones"][0]["environment"].as<u64>(0),
              e1.get_ref<quoll::Id>()->id);
  }

  {
    writer.deleteEntities({e1});
    auto node = loadSceneFile();
    EXPECT_TRUE(node["zones"][0]["environment"].IsNull());
  }
}

TEST_F(SceneWriterTest,
       DoesNotSaveEntityAsInitialCameraIfItDoesNotHaveCameraComponent) {
  auto entity = scene.entityDatabase.entity();
  writer.syncEntities({entity});

  scene.activeCamera = entity;
  writer.syncScene();

  auto node = loadSceneFile();
  EXPECT_FALSE(node["zones"][0]["startingCamera"]);
}

TEST_F(SceneWriterTest,
       DoesNotSaveEntityAsInitialCameraIfItDoesNotHaveIdComponent) {
  auto entity = scene.entityDatabase.entity();
  entity.set<quoll::PerspectiveLens>({});

  scene.activeCamera = entity;
  writer.syncScene();

  auto node = loadSceneFile();
  EXPECT_FALSE(node["zones"][0]["startingCamera"]);
}

TEST_F(SceneWriterTest, SavesEntityAsStartingCameraIfItHasCameraComponent) {
  auto entity = scene.entityDatabase.entity();
  entity.set<quoll::Id>({15});
  entity.set<quoll::PerspectiveLens>({});
  writer.syncEntities({entity});

  scene.activeCamera = entity;
  writer.syncScene();

  auto node = loadSceneFile();
  EXPECT_EQ(node["zones"][0]["startingCamera"].as<u64>(0), 15);
}

TEST_F(SceneWriterTest, DoesNotSaveEntityAsEnvironmentIfItDoesNotHaveId) {
  scene.activeEnvironment = scene.entityDatabase.entity();
  writer.syncScene();

  auto node = loadSceneFile();
  EXPECT_FALSE(node["zones"][0]["environment"].IsNull());
}

TEST_F(SceneWriterTest, SetsEnvironmentToEntityIdIfEnvironmentEntityHasId) {
  scene.activeEnvironment = scene.entityDatabase.entity();
  scene.activeEnvironment.set<quoll::Id>({10});

  writer.syncScene();

  auto node = loadSceneFile();
  auto envNode = node["zones"][0]["environment"];

  EXPECT_EQ(envNode.as<u64>(0), 10);
}
