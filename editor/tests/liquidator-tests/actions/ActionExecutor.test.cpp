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
  TestAction(std::vector<liquid::Entity> entitiesToSave,
             std::vector<liquid::Entity> entitiesToDelete)
      : mEntitiesToSave(entitiesToSave), mEntitiesToDelete(entitiesToDelete) {}

  liquid::editor::ActionExecutorResult
  onExecute(liquid::editor::WorkspaceState &state) {
    mCalled = true;

    liquid::editor::ActionExecutorResult res{};

    res.entitiesToSave = mEntitiesToSave;
    res.entitiesToDelete = mEntitiesToDelete;

    return res;
  }

  bool predicate(liquid::editor::WorkspaceState &state) { return true; }

  bool isCalled() { return mCalled; }

private:
  bool mCalled = false;
  std::vector<liquid::Entity> mEntitiesToSave;
  std::vector<liquid::Entity> mEntitiesToDelete;
};

TEST_F(ActionExecutorTest, ExecuteCallsActionExecutorWithState) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  auto *actionPtr = new TestAction({}, {});
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

  auto *actionPtr = new TestAction({entity}, {});
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

  auto *actionPtr = new TestAction({entity}, {});
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

  auto *actionPtr = new TestAction({}, {entity});
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

  auto *actionPtr = new TestAction({}, {entity});
  std::unique_ptr<liquid::editor::Action> action(actionPtr);

  executor.execute(action);
  EXPECT_TRUE(actionPtr->isCalled());
  EXPECT_TRUE(std::filesystem::exists(entityPath));
}
