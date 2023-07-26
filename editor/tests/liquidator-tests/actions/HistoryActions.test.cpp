#include "liquid/core/Base.h"
#include "liquidator/actions/HistoryActions.h"
#include "liquidator/actions/ActionExecutor.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using UndoActionTest = ActionTestBase;

struct HistoryTestActionData {
  bool called = false;
  bool undoCalled = false;
};

class HistoryTestAction : public liquid::editor::Action {
public:
  HistoryTestAction() : mData(new HistoryTestActionData) {}

  liquid::editor::ActionExecutorResult
  onExecute(liquid::editor::WorkspaceState &state,
            liquid::AssetRegistry &assetRegistry) override {
    mData->called = true;

    liquid::editor::ActionExecutorResult res;
    res.addToHistory = true;
    return res;
  }

  liquid::editor::ActionExecutorResult
  onUndo(liquid::editor::WorkspaceState &state,
         liquid::AssetRegistry &assetRegistry) override {
    mData->undoCalled = true;

    return {};
  }

  bool predicate(liquid::editor::WorkspaceState &state,
                 liquid::AssetRegistry &assetRegistry) {
    return true;
  }

  liquid::SharedPtr<HistoryTestActionData> getData() { return mData; }

private:
  liquid::SharedPtr<HistoryTestActionData> mData;
};

TEST_F(UndoActionTest, ExecutorCallsActionExecutorUndo) {
  auto *action = new HistoryTestAction;
  auto actionData = action->getData();

  liquid::editor::ActionExecutor executor(state, assetRegistry,
                                          std::filesystem::current_path());

  executor.execute(std::unique_ptr<liquid::editor::Action>(action));
  executor.process();
  EXPECT_TRUE(actionData->called);

  executor.execute<liquid::editor::Undo>(executor);
  executor.process();

  EXPECT_TRUE(actionData->undoCalled);
}

TEST_F(UndoActionTest,
       PredicateReturnsTrueIfActionExecutorUndoStackIsNotEmpty) {
  auto *action = new HistoryTestAction;
  auto actionData = action->getData();

  liquid::editor::ActionExecutor executor(state, assetRegistry,
                                          std::filesystem::current_path());

  executor.execute(std::unique_ptr<liquid::editor::Action>(action));
  executor.process();
  EXPECT_TRUE(actionData->called);

  EXPECT_TRUE(liquid::editor::Undo(executor).predicate(state, assetRegistry));
}

TEST_F(UndoActionTest, PredicateReturnsFalseIfActionExecutorUndoStackIsEmpty) {
  liquid::editor::ActionExecutor executor(state, assetRegistry,
                                          std::filesystem::current_path());

  EXPECT_FALSE(liquid::editor::Undo(executor).predicate(state, assetRegistry));
}

using RedoActionTest = ActionTestBase;

TEST_F(RedoActionTest, ExecutorCallsActionExecutor) {
  auto *action = new HistoryTestAction;
  auto actionData = action->getData();

  liquid::editor::ActionExecutor executor(state, assetRegistry,
                                          std::filesystem::current_path());

  executor.execute(std::unique_ptr<liquid::editor::Action>(action));
  executor.process();
  EXPECT_TRUE(actionData->called);

  executor.undo();
  EXPECT_TRUE(actionData->undoCalled);

  actionData->called = false;

  executor.execute<liquid::editor::Redo>(executor);
  executor.process();

  EXPECT_TRUE(actionData->called);
}

TEST_F(RedoActionTest,
       PredicateReturnsTrueIfActionExecutorRedoStackIsNotEmpty) {
  auto *action = new HistoryTestAction;
  auto actionData = action->getData();

  liquid::editor::ActionExecutor executor(state, assetRegistry,
                                          std::filesystem::current_path());

  executor.execute(std::unique_ptr<liquid::editor::Action>(action));
  executor.process();

  executor.undo();
  EXPECT_TRUE(actionData->undoCalled);

  EXPECT_TRUE(liquid::editor::Redo(executor).predicate(state, assetRegistry));
}

TEST_F(RedoActionTest, PredicateReturnsFalseIfActionExecutorRedoStackIsEmpty) {
  liquid::editor::ActionExecutor executor(state, assetRegistry,
                                          std::filesystem::current_path());

  EXPECT_FALSE(liquid::editor::Redo(executor).predicate(state, assetRegistry));
}
