#include "quoll/core/Base.h"
#include "quoll/core/Id.h"
#include "quoll/core/Name.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll-tests/Testing.h"

class TestAssetSyncer : public quoll::editor::AssetSyncer {
public:
  void syncEntities(const std::vector<quoll::Entity> &entities) override {
    syncedEntities = entities;
  }

  void deleteEntities(const std::vector<quoll::Entity> &entities) override {
    deletedEntities = entities;
  }

  void syncScene() override { syncedScene = true; }

public:
  std::vector<quoll::Entity> syncedEntities;
  std::vector<quoll::Entity> deletedEntities;
  bool syncedScene = false;
};

const quoll::Path CachePath = std::filesystem::current_path() / "cache";

class ActionExecutorTest : public ::testing::Test {
public:
  ActionExecutorTest() : assetCache(CachePath) {}

  void SetUp() override { executor.setAssetSyncer(&assetSyncer); }

public:
  quoll::AssetCache assetCache;
  quoll::editor::WorkspaceState state{};
  TestAssetSyncer assetSyncer;
  quoll::editor::ActionExecutor executor{state, assetCache};
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
            quoll::AssetCache &assetCache) override {
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
         quoll::AssetCache &assetCache) override {
    mData->undoCalled = true;

    quoll::editor::ActionExecutorResult res{};

    res.entitiesToSave = mData->entitiesToDelete;
    res.entitiesToDelete = mData->entitiesToSave;
    res.saveScene = mData->saveScene;

    return res;
  }

  bool predicate(quoll::editor::WorkspaceState &state,
                 quoll::AssetCache &assetCache) override {
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

  auto *actionPtr = new TestAction;
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();
  EXPECT_TRUE(actionData->called);
  EXPECT_TRUE(executor.getUndoStack().empty());
  EXPECT_TRUE(executor.getRedoStack().empty());
}

TEST_F(ActionExecutorTest,
       ExecuteSyncsEntitiesWithSyncerIfActionReturnsEntitiesToSave) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"My name"});

  auto *actionPtr = new TestAction;
  actionPtr->saveEntityOnExecute(entity);
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();

  EXPECT_TRUE(actionData->called);
  EXPECT_EQ(assetSyncer.syncedEntities.size(), 1);
  EXPECT_EQ(assetSyncer.syncedEntities.at(0), entity);
  EXPECT_EQ(assetSyncer.deletedEntities.size(), 0);
  EXPECT_EQ(assetSyncer.syncedScene, false);
}

TEST_F(ActionExecutorTest,
       ExecuteDeletesEntitiesWithSyncerIfActionReturnsEntitiesToDelete) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"My name"});
  state.scene.entityDatabase.set<quoll::Id>(entity, {15});

  auto *actionPtr = new TestAction;
  actionPtr->deleteEntityOnExecute(entity);
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();

  EXPECT_TRUE(actionData->called);
  EXPECT_EQ(assetSyncer.deletedEntities.size(), 1);
  EXPECT_EQ(assetSyncer.deletedEntities.at(0), entity);
  EXPECT_EQ(assetSyncer.syncedScene, false);
  EXPECT_EQ(assetSyncer.syncedEntities.size(), 0);
}

TEST_F(ActionExecutorTest,
       ExecuteSyncsSceneWithSyncerIfActionReturnsSaveScene) {
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
  EXPECT_EQ(assetSyncer.syncedScene, true);
  EXPECT_EQ(assetSyncer.deletedEntities.size(), 0);
  EXPECT_EQ(assetSyncer.syncedEntities.size(), 0);
}

TEST_F(ActionExecutorTest,
       ExecuteAddsActionToUndoStackIfActionReturnsAddToHistory) {
  auto *actionPtr = new TestAction;
  actionPtr->addToHistory();
  auto actionData = actionPtr->getData();

  executor.execute(std::unique_ptr<quoll::editor::Action>(actionPtr));
  executor.process();
  EXPECT_TRUE(actionData->called);

  EXPECT_FALSE(executor.getUndoStack().empty());
  EXPECT_TRUE(executor.getRedoStack().empty());
}

TEST_F(ActionExecutorTest, UndoDoesNothingIfUndoStackIsEmpty) {
  executor.undo();
}

TEST_F(ActionExecutorTest, UndoCallsLastActionUndoAndUpdatesRedoStack) {
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

TEST_F(ActionExecutorTest, RedoCallsLastActionExecuteAndUpdatesUndoStack) {
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

TEST_F(ActionExecutorTest,
       ExecuteAfterUndoDoesNotClearRedoStackIfActionDoesNotReturnAddToHistory) {
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

TEST_F(ActionExecutorTest,
       ExecuteAfterUndoClearsRedoStackIfActionReturnsAddToHistory) {
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
