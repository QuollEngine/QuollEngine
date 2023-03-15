#include "liquid/core/Base.h"
#include "liquidator/actions/ActionExecutor.h"

#include "liquidator-tests/Testing.h"

static const liquid::Path ScenePath{std::filesystem::current_path() / "scene"};

class ActionExecutorTest : public ::testing::Test {
public:
  void SetUp() override {
    std::filesystem::create_directories(ScenePath / "entities");

    YAML::Node scene;
    scene["persistentZone"] = 0;
    scene["zones"][0]["entities"] = (ScenePath / "entities").string();
    std::ofstream stream(ScenePath / "main.lqscene");
    stream << scene;
    stream.close();

    executor.getSceneIO().loadScene(ScenePath / "main.lqscene");
  }

  void TearDown() override { std::filesystem::remove_all(ScenePath); }

public:
  liquid::AssetRegistry registry;
  liquid::editor::WorkspaceState state{{}, registry};
  liquid::editor::ActionExecutor executor{state, ScenePath / "main.lqscene"};
};

class TestAction : public liquid::editor::Action {
public:
  liquid::editor::ActionExecutorResult
  onExecute(liquid::editor::WorkspaceState &state) {
    mCalled = true;

    liquid::editor::ActionExecutorResult res{};

    res.entitiesToSave = mEntitiesToSave;
    res.entitiesToDelete = mEntitiesToDelete;
    res.saveScene = mSaveScene;

    return res;
  }

  void saveEntityOnExecute(liquid::Entity entity) {
    mEntitiesToSave.push_back(entity);
  }

  void deleteEntityOnExecute(liquid::Entity entity) {
    mEntitiesToDelete.push_back(entity);
  }

  void saveSceneOnExecute() { mSaveScene = true; }

  void setPredicate(bool predicate) { mPredicate = predicate; }

  bool predicate(liquid::editor::WorkspaceState &state) { return mPredicate; }

  bool isCalled() { return mCalled; }

private:
  bool mCalled = false;
  std::vector<liquid::Entity> mEntitiesToSave;
  std::vector<liquid::Entity> mEntitiesToDelete;
  bool mSaveScene = false;
  bool mPredicate = true;
};

TEST_F(ActionExecutorTest,
       ExecuteDoesNotCallActionExecutorIfActionPredicateReturnsFalse) {
  auto *actionPtr = new TestAction;
  actionPtr->setPredicate(false);
  std::unique_ptr<liquid::editor::Action> action(actionPtr);

  executor.execute(action);
  EXPECT_FALSE(actionPtr->isCalled());
}

TEST_F(ActionExecutorTest, ExecuteCallsActionExecutorWithState) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  auto *actionPtr = new TestAction;
  std::unique_ptr<liquid::editor::Action> action(actionPtr);

  executor.execute(action);
  EXPECT_TRUE(actionPtr->isCalled());
}

TEST_F(ActionExecutorTest,
       ExecuteCreatesEntityFilesIfActionReturnsEntitiesToSaveAndModeIsEdit) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::Name>(entity, {"My name"});

  auto entityPath = ScenePath / "entities" / "1.lqnode";
  EXPECT_FALSE(std::filesystem::exists(entityPath));

  auto *actionPtr = new TestAction;
  actionPtr->saveEntityOnExecute(entity);

  std::unique_ptr<liquid::editor::Action> action(actionPtr);
  executor.execute(action);
  EXPECT_TRUE(actionPtr->isCalled());
  EXPECT_TRUE(std::filesystem::exists(entityPath));
}

TEST_F(
    ActionExecutorTest,
    ExecuteDoesNotCreateEntityFilesIfActionReturnsEntitiesToSaveAndModeIsSimulation) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::Name>(entity, {"My name"});

  auto entityPath = ScenePath / "entities" / "1.lqnode";
  EXPECT_FALSE(std::filesystem::exists(entityPath));

  auto *actionPtr = new TestAction;
  actionPtr->saveEntityOnExecute(entity);

  std::unique_ptr<liquid::editor::Action> action(actionPtr);
  executor.execute(action);
  EXPECT_TRUE(actionPtr->isCalled());
  EXPECT_FALSE(std::filesystem::exists(entityPath));
}

TEST_F(ActionExecutorTest,
       ExecuteDeletesEntityFilesIfActionReturnsEntitiesToDeleteAndModeIsEdit) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::Name>(entity, {"My name"});
  state.scene.entityDatabase.set<liquid::Id>(entity, {15});

  executor.getSceneIO().saveEntity(entity, ScenePath / "main.lqscene");

  auto entityPath = ScenePath / "entities" / "15.lqnode";
  EXPECT_TRUE(std::filesystem::exists(entityPath));

  auto *actionPtr = new TestAction;
  actionPtr->deleteEntityOnExecute(entity);

  std::unique_ptr<liquid::editor::Action> action(actionPtr);
  executor.execute(action);
  EXPECT_TRUE(actionPtr->isCalled());
  EXPECT_FALSE(std::filesystem::exists(entityPath));
}

TEST_F(
    ActionExecutorTest,
    ExecuteDoesNotDeleteEntityFilesIfActionReturnsEntitiesToDeleteAndModeIsSimulation) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::Name>(entity, {"My name"});
  state.scene.entityDatabase.set<liquid::Id>(entity, {15});

  executor.getSceneIO().saveEntity(entity, ScenePath / "main.lqscene");

  auto entityPath = ScenePath / "entities" / "15.lqnode";
  EXPECT_TRUE(std::filesystem::exists(entityPath));

  auto *actionPtr = new TestAction;
  actionPtr->deleteEntityOnExecute(entity);

  std::unique_ptr<liquid::editor::Action> action(actionPtr);
  executor.execute(action);
  EXPECT_TRUE(actionPtr->isCalled());
  EXPECT_TRUE(std::filesystem::exists(entityPath));
}

TEST_F(ActionExecutorTest,
       ExecuteSavesSceneFileIfActionReturnsSaveSceneAndModeIsEdit) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  state.scene.entityDatabase.set<liquid::Id>(entity, {15});
  state.scene.activeCamera = entity;

  executor.getSceneIO().saveEntity(entity, ScenePath / "main.lqscene");

  auto entityPath = ScenePath / "entities" / "15.lqnode";
  EXPECT_TRUE(std::filesystem::exists(entityPath));

  auto *actionPtr = new TestAction;
  actionPtr->saveSceneOnExecute();

  std::unique_ptr<liquid::editor::Action> action(actionPtr);
  executor.execute(action);
  EXPECT_TRUE(actionPtr->isCalled());

  std::ifstream stream(ScenePath / "main.lqscene");
  auto node = YAML::Load(stream);
  stream.close();

  auto startingCamera =
      node["zones"][node["persistentZone"].as<size_t>()]["startingCamera"];
  EXPECT_TRUE(startingCamera);
  EXPECT_TRUE(startingCamera.IsScalar());
  EXPECT_EQ(startingCamera.as<uint32_t>(), 15);
}

TEST_F(ActionExecutorTest,
       ExecuteDoesNotSaveSceneFileIfActionReturnsSaveSceneAndModeIsSimulation) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  state.scene.entityDatabase.set<liquid::Id>(entity, {15});
  state.scene.activeCamera = entity;

  executor.getSceneIO().saveEntity(entity, ScenePath / "main.lqscene");

  auto entityPath = ScenePath / "entities" / "15.lqnode";
  EXPECT_TRUE(std::filesystem::exists(entityPath));

  auto *actionPtr = new TestAction;
  actionPtr->saveSceneOnExecute();

  std::unique_ptr<liquid::editor::Action> action(actionPtr);
  executor.execute(action);
  EXPECT_TRUE(actionPtr->isCalled());

  std::ifstream stream(ScenePath / "main.lqscene");
  auto node = YAML::Load(stream);
  stream.close();

  auto startingCamera =
      node["zones"][node["persistentZone"].as<size_t>()]["startingCamera"];
  EXPECT_FALSE(startingCamera);
}
