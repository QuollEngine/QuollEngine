#include "quoll/core/Base.h"
#include "quoll/core/Id.h"
#include "quoll/core/Name.h"
#include "quoll/scene/PerspectiveLens.h"

#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll/editor/asset/SceneWriter.h"

#include "quoll/editor-tests/Testing.h"

static const quoll::Path ScenePath{std::filesystem::current_path() /
                                   "scene-test" / "main.scene"};

class ActionExecutorTest : public ::testing::Test {
public:
  void SetUp() override {
    TearDown();
    std::filesystem::create_directories(ScenePath.parent_path());

    YAML::Node root;
    root["name"] = "TestScene";
    root["version"] = "0.1";

    YAML::Node zoneNode;
    zoneNode["name"] = "TestZone";
    root["zones"][0] = zoneNode;

    std::ofstream stream(ScenePath);
    stream << root;
    stream.close();

    sceneWriter.open(ScenePath);

    executor.setAssetSyncer(&sceneWriter);
  }

  void TearDown() override {
    std::filesystem::remove_all(ScenePath.parent_path());
  }

public:
  quoll::AssetRegistry assetRegistry;
  quoll::editor::WorkspaceState state{};
  quoll::editor::SceneWriter sceneWriter{state.scene, assetRegistry};
  quoll::editor::ActionExecutor executor{state, assetRegistry};
};

struct TestActionData {
  bool called = false;
  bool undoCalled = false;
  std::vector<quoll::Entity> entitiesToSave;
  std::vector<quoll::Entity> entitiesToDelete;
  bool saveScene = false;
  bool mPredicate = true;
  bool addToHistory = false;
};

class TestAction : public quoll::editor::Action {
public:
  TestAction() : mData(new TestActionData) {}

  quoll::editor::ActionExecutorResult
  onExecute(quoll::editor::WorkspaceState &state,
            quoll::AssetRegistry &assetRegistry) {
    mData->called = true;

    quoll::editor::ActionExecutorResult res{};

    res.entitiesToSave = mData->entitiesToSave;
    res.entitiesToDelete = mData->entitiesToDelete;
    res.saveScene = mData->saveScene;
    res.addToHistory = mData->addToHistory;

    return res;
  }

  quoll::editor::ActionExecutorResult
  onUndo(quoll::editor::WorkspaceState &state,
         quoll::AssetRegistry &assetRegistry) override {
    mData->undoCalled = true;

    quoll::editor::ActionExecutorResult res{};

    res.entitiesToSave = mData->entitiesToDelete;
    res.entitiesToDelete = mData->entitiesToSave;
    res.saveScene = mData->saveScene;

    return res;
  }

  bool predicate(quoll::editor::WorkspaceState &state,
                 quoll::AssetRegistry &assetRegistry) override {
    return mData->mPredicate;
  }

  inline quoll::SharedPtr<TestActionData> getData() { return mData; }

public:
  void saveEntityOnExecute(quoll::Entity entity) {
    mData->entitiesToSave.push_back(entity);
  }

  void deleteEntityOnExecute(quoll::Entity entity) {
    mData->entitiesToDelete.push_back(entity);
  }

  void saveSceneOnExecute() { mData->saveScene = true; }

  void addToHistory() { mData->addToHistory = true; }

  void setPredicate(bool predicate) { mData->mPredicate = predicate; }

private:
  quoll::SharedPtr<TestActionData> mData;
};

TEST_F(ActionExecutorTest, ExecuteDoesNothingIfNoActionToProcess) {
  // Ensure that function does not
  // fail if there are no actions to
  // process
  executor.process();
}

TEST_F(ActionExecutorTest,
       ExecuteDoesNotCallActionExecutorIfActionPredicateReturnsFalse) {
  auto *actionPtr = new TestAction;
  actionPtr->setPredicate(false);

  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();
  EXPECT_FALSE(actionData->called);
}

TEST_F(ActionExecutorTest, ExecuteCallsActionExecutorWithState) {
  state.mode = quoll::editor::WorkspaceMode::Simulation;

  auto *actionPtr = new TestAction;
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();
  EXPECT_TRUE(actionData->called);
  EXPECT_TRUE(executor.getUndoStack().empty());
  EXPECT_TRUE(executor.getRedoStack().empty());
}

TEST_F(ActionExecutorTest, ProcessDoesNotBreakIfNoAssetSyncer) {
  executor.setAssetSyncer(nullptr);

  state.mode = quoll::editor::WorkspaceMode::Simulation;

  auto *actionPtr = new TestAction;
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();
  EXPECT_TRUE(actionData->called);
  EXPECT_TRUE(executor.getUndoStack().empty());
  EXPECT_TRUE(executor.getRedoStack().empty());
}

TEST_F(ActionExecutorTest,
       ExecuteCreatesEntityFilesIfActionReturnsEntitiesToSaveAndModeIsEdit) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"My name"});

  {
    std::ifstream stream(ScenePath);
    auto node = YAML::Load(stream);
    stream.close();

    EXPECT_EQ(node["entities"].size(), 0);
  }

  auto *actionPtr = new TestAction;
  actionPtr->saveEntityOnExecute(entity);
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();
  EXPECT_TRUE(actionData->called);

  {
    std::ifstream stream(ScenePath);
    auto node = YAML::Load(stream);
    stream.close();

    EXPECT_EQ(node["entities"].size(), 1);
    auto id = state.scene.entityDatabase.get<quoll::Id>(entity).id;
    EXPECT_EQ(node["entities"][0]["id"].as<u64>(0), id);
  }
}

TEST_F(
    ActionExecutorTest,
    ExecuteDoesNotCreateEntityFilesIfActionReturnsEntitiesToSaveAndModeIsSimulation) {
  state.mode = quoll::editor::WorkspaceMode::Simulation;

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"My name"});

  {
    std::ifstream stream(ScenePath);
    auto node = YAML::Load(stream);
    stream.close();
    EXPECT_EQ(node["entities"].size(), 0);
  }

  auto *actionPtr = new TestAction;
  actionPtr->saveEntityOnExecute(entity);
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();
  EXPECT_TRUE(actionData->called);

  {
    std::ifstream stream(ScenePath);
    auto node = YAML::Load(stream);
    stream.close();
    EXPECT_EQ(node["entities"].size(), 0);
  }
}

TEST_F(ActionExecutorTest,
       ExecuteDeletesEntityFilesIfActionReturnsEntitiesToDeleteAndModeIsEdit) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"My name"});
  state.scene.entityDatabase.set<quoll::Id>(entity, {15});

  sceneWriter.syncEntities({entity});

  auto *actionPtr = new TestAction;
  actionPtr->deleteEntityOnExecute(entity);
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();
  EXPECT_TRUE(actionData->called);
  {
    std::ifstream stream(ScenePath);
    auto node = YAML::Load(stream);
    stream.close();
    EXPECT_EQ(node["entities"].size(), 0);
  }
}

TEST_F(
    ActionExecutorTest,
    ExecuteDoesNotDeleteEntityFilesIfActionReturnsEntitiesToDeleteAndModeIsSimulation) {
  state.mode = quoll::editor::WorkspaceMode::Simulation;

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"My name"});
  state.scene.entityDatabase.set<quoll::Id>(entity, {15});

  sceneWriter.syncEntities({entity});

  auto *actionPtr = new TestAction;
  actionPtr->deleteEntityOnExecute(entity);
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();
  EXPECT_TRUE(actionData->called);

  {
    std::ifstream stream(ScenePath);
    auto node = YAML::Load(stream);
    stream.close();
    EXPECT_EQ(node["entities"].size(), 1);
  }
}

TEST_F(ActionExecutorTest,
       ExecuteSavesSceneFileIfActionReturnsSaveSceneAndModeIsEdit) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::PerspectiveLens>(entity, {});
  state.scene.entityDatabase.set<quoll::Id>(entity, {15});
  state.scene.activeCamera = entity;

  sceneWriter.syncEntities({entity});

  auto *actionPtr = new TestAction;
  actionPtr->saveSceneOnExecute();
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();
  EXPECT_TRUE(actionData->called);

  std::ifstream stream(ScenePath);
  auto node = YAML::Load(stream);
  stream.close();

  auto startingCamera = node["zones"][0]["startingCamera"];
  EXPECT_TRUE(startingCamera);
  EXPECT_TRUE(startingCamera.IsScalar());
  EXPECT_EQ(startingCamera.as<u32>(0), 15);
}

TEST_F(ActionExecutorTest,
       ExecuteDoesNotSaveSceneFileIfActionReturnsSaveSceneAndModeIsSimulation) {
  state.mode = quoll::editor::WorkspaceMode::Simulation;

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::PerspectiveLens>(entity, {});
  state.scene.entityDatabase.set<quoll::Id>(entity, {15});
  state.scene.activeCamera = entity;

  auto *actionPtr = new TestAction;
  actionPtr->saveSceneOnExecute();
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();
  EXPECT_TRUE(actionData->called);

  std::ifstream stream(ScenePath);
  auto node = YAML::Load(stream);
  stream.close();

  auto startingCamera = node["zones"][0]["startingCamera"];
  EXPECT_FALSE(startingCamera);
}

TEST_F(ActionExecutorTest,
       ExecuteAddsActionToUndoStackIfActionReturnsAddToHistoryAndModeIsEdit) {
  auto *actionPtr = new TestAction;
  actionPtr->addToHistory();
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();
  EXPECT_TRUE(actionData->called);

  EXPECT_FALSE(executor.getUndoStack().empty());
  EXPECT_TRUE(executor.getRedoStack().empty());
}

TEST_F(
    ActionExecutorTest,
    ExecuteDoesNotAddActionToUndoStackIfActionReturnsAddToHistoryAndModeIsSimulation) {
  state.mode = quoll::editor::WorkspaceMode::Simulation;

  auto *actionPtr = new TestAction;
  actionPtr->addToHistory();
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();
  EXPECT_TRUE(actionData->called);

  EXPECT_TRUE(executor.getUndoStack().empty());
  EXPECT_TRUE(executor.getRedoStack().empty());
}

TEST_F(ActionExecutorTest, UndoDoesNothingIfUndoStackIsEmpty) {
  executor.undo();
}

TEST_F(ActionExecutorTest, UndoDoesNothingIfModeIsSimulation) {
  auto *actionPtr = new TestAction;
  actionPtr->addToHistory();
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();
  EXPECT_TRUE(actionData->called);
  EXPECT_FALSE(executor.getUndoStack().empty());
  EXPECT_TRUE(executor.getRedoStack().empty());

  state.mode = quoll::editor::WorkspaceMode::Simulation;
  executor.undo();

  EXPECT_FALSE(executor.getUndoStack().empty());
  EXPECT_TRUE(executor.getRedoStack().empty());
}

TEST_F(ActionExecutorTest,
       UndoCallsLastActionUndoAndUpdatesRedoStackIfModeIsEdit) {
  auto *actionPtr = new TestAction;
  actionPtr->addToHistory();
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();
  EXPECT_TRUE(actionData->called);
  EXPECT_FALSE(executor.getUndoStack().empty());
  EXPECT_TRUE(executor.getRedoStack().empty());

  executor.undo();

  EXPECT_TRUE(actionData->undoCalled);
  EXPECT_TRUE(executor.getUndoStack().empty());
  EXPECT_FALSE(executor.getRedoStack().empty());
}

TEST_F(ActionExecutorTest, RedoDoesNothingIfRedoStackIsEmpty) {
  executor.redo();
}

TEST_F(ActionExecutorTest, RedoDoesNothingIfModeIsSimulation) {
  auto *actionPtr = new TestAction;
  actionPtr->addToHistory();
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();
  EXPECT_TRUE(actionData->called);
  EXPECT_FALSE(executor.getUndoStack().empty());
  EXPECT_TRUE(executor.getRedoStack().empty());

  executor.undo();
  EXPECT_TRUE(executor.getUndoStack().empty());
  EXPECT_FALSE(executor.getRedoStack().empty());

  state.mode = quoll::editor::WorkspaceMode::Simulation;
  executor.redo();
  EXPECT_TRUE(executor.getUndoStack().empty());
  EXPECT_FALSE(executor.getRedoStack().empty());
}

TEST_F(ActionExecutorTest,
       RedoCallsLastActionExecuteAndUpdatesUndoStackifModeIsEdit) {
  auto *actionPtr = new TestAction;
  actionPtr->addToHistory();
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();
  EXPECT_TRUE(actionData->called);
  EXPECT_FALSE(executor.getUndoStack().empty());
  EXPECT_TRUE(executor.getRedoStack().empty());

  actionData->called = false;

  executor.undo();
  EXPECT_TRUE(executor.getUndoStack().empty());
  EXPECT_FALSE(executor.getRedoStack().empty());

  executor.redo();
  EXPECT_TRUE(actionData->called);
  EXPECT_FALSE(executor.getUndoStack().empty());
  EXPECT_TRUE(executor.getRedoStack().empty());
}

TEST_F(
    ActionExecutorTest,
    ExecuteAfterUndoDoesNotClearRedoStackIfActionDoesNotReturnAddToHistoryAndModeIsEdit) {
  {
    auto *actionPtr = new TestAction;
    actionPtr->addToHistory();
    auto actionData = actionPtr->getData();

    executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
    executor.process();
    EXPECT_TRUE(actionData->called);
    EXPECT_FALSE(executor.getUndoStack().empty());
    EXPECT_TRUE(executor.getRedoStack().empty());

    actionData->called = false;

    executor.undo();
    EXPECT_TRUE(executor.getUndoStack().empty());
    EXPECT_EQ(executor.getRedoStack().size(), 1);
  }

  {
    auto *actionPtr = new TestAction;
    auto actionData = actionPtr->getData();

    executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
    executor.process();

    EXPECT_TRUE(actionData->called);
    EXPECT_EQ(executor.getUndoStack().size(), 0);
    EXPECT_EQ(executor.getRedoStack().size(), 1);
  }
}

TEST_F(
    ActionExecutorTest,
    ExecuteAfterUndoClearsRedoStackIfActionReturnsAddToHistoryAndModeIsEdit) {
  {
    auto *actionPtr = new TestAction;
    actionPtr->addToHistory();
    auto actionData = actionPtr->getData();

    executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
    executor.process();
    EXPECT_TRUE(actionData->called);
    EXPECT_FALSE(executor.getUndoStack().empty());
    EXPECT_TRUE(executor.getRedoStack().empty());

    executor.undo();
    EXPECT_TRUE(executor.getUndoStack().empty());
    EXPECT_FALSE(executor.getRedoStack().empty());
  }

  {
    auto *actionPtr = new TestAction;
    actionPtr->addToHistory();
    auto actionData = actionPtr->getData();

    executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
    executor.process();

    EXPECT_TRUE(actionData->called);
    EXPECT_EQ(executor.getUndoStack().size(), 1);
    EXPECT_TRUE(executor.getRedoStack().empty());
  }
}
