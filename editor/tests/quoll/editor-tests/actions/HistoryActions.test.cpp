#include "quoll/core/Base.h"
#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll/editor/actions/HistoryActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

using UndoActionTest = ActionTestBase;

struct HistoryTestActionData {
  bool called = false;
  bool undoCalled = false;
};

class HistoryTestAction : public quoll::editor::Action {
public:
  HistoryTestAction() : mData(new HistoryTestActionData) {}

  quoll::editor::ActionExecutorResult
  onExecute(quoll::editor::WorkspaceState &state,
            quoll::AssetRegistry &assetRegistry) override {
    mData->called = true;

    quoll::editor::ActionExecutorResult res;
    res.addToHistory = true;
    return res;
  }

  quoll::editor::ActionExecutorResult
  onUndo(quoll::editor::WorkspaceState &state,
         quoll::AssetRegistry &assetRegistry) override {
    mData->undoCalled = true;

    return {};
  }

  bool predicate(quoll::editor::WorkspaceState &state,
                 quoll::AssetRegistry &assetRegistry) override {
    return true;
  }

  quoll::SharedPtr<HistoryTestActionData> getData() { return mData; }

private:
  quoll::SharedPtr<HistoryTestActionData> mData;
};

TEST_F(UndoActionTest, ExecutorCallsActionExecutorUndo) {
  auto *action = new HistoryTestAction;
  auto actionData = action->getData();

  quoll::editor::ActionExecutor executor(state, assetRegistry);

  executor.execute(std::unique_ptr<quoll::editor::Action>(action));
  executor.process();
  EXPECT_TRUE(actionData->called);

  executor.execute<quoll::editor::Undo>(executor);
  executor.process();

  EXPECT_TRUE(actionData->undoCalled);
}

TEST_F(UndoActionTest,
       PredicateReturnsTrueIfActionExecutorUndoStackIsNotEmpty) {
  auto *action = new HistoryTestAction;
  auto actionData = action->getData();

  quoll::editor::ActionExecutor executor(state, assetRegistry);

  executor.execute(std::unique_ptr<quoll::editor::Action>(action));
  executor.process();
  EXPECT_TRUE(actionData->called);

  EXPECT_TRUE(quoll::editor::Undo(executor).predicate(state, assetRegistry));
}

TEST_F(UndoActionTest, PredicateReturnsFalseIfActionExecutorUndoStackIsEmpty) {
  quoll::editor::ActionExecutor executor(state, assetRegistry);

  EXPECT_FALSE(quoll::editor::Undo(executor).predicate(state, assetRegistry));
}

using RedoActionTest = ActionTestBase;

TEST_F(RedoActionTest, ExecutorCallsActionExecutor) {
  auto *action = new HistoryTestAction;
  auto actionData = action->getData();

  quoll::editor::ActionExecutor executor(state, assetRegistry);

  executor.execute(std::unique_ptr<quoll::editor::Action>(action));
  executor.process();
  EXPECT_TRUE(actionData->called);

  executor.undo();
  EXPECT_TRUE(actionData->undoCalled);

  actionData->called = false;

  executor.execute<quoll::editor::Redo>(executor);
  executor.process();

  EXPECT_TRUE(actionData->called);
}

TEST_F(RedoActionTest,
       PredicateReturnsTrueIfActionExecutorRedoStackIsNotEmpty) {
  auto *action = new HistoryTestAction;
  auto actionData = action->getData();

  quoll::editor::ActionExecutor executor(state, assetRegistry);

  executor.execute(std::unique_ptr<quoll::editor::Action>(action));
  executor.process();

  executor.undo();
  EXPECT_TRUE(actionData->undoCalled);

  EXPECT_TRUE(quoll::editor::Redo(executor).predicate(state, assetRegistry));
}

TEST_F(RedoActionTest, PredicateReturnsFalseIfActionExecutorRedoStackIsEmpty) {
  quoll::editor::ActionExecutor executor(state, assetRegistry);

  EXPECT_FALSE(quoll::editor::Redo(executor).predicate(state, assetRegistry));
}
