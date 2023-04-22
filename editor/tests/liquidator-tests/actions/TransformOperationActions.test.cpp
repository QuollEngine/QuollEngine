#include "liquid/core/Base.h"
#include "liquidator/actions/TransformOperationActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using TO = liquid::editor::TransformOperation;

using SetActiveTransformActionTest = ActionTestBase;

TEST_F(SetActiveTransformActionTest,
       ExecutorChangesActiveTransformToProvidedTransform) {
  state.activeTransform = TO::Rotate;

  liquid::editor::SetActiveTransform action(TO::Move);
  auto res = action.onExecute(state);

  EXPECT_EQ(state.activeTransform, TO::Move);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(SetActiveTransformActionTest,
       UndoChangesActiveTransformToPreviousTransform) {
  state.activeTransform = TO::Rotate;

  liquid::editor::SetActiveTransform action(TO::Move);
  action.onExecute(state);
  action.onUndo(state);

  EXPECT_EQ(state.activeTransform, TO::Rotate);
}

TEST_F(SetActiveTransformActionTest,
       PredicateReturnsFalseIfActiveTransformEqualsProvidedTransform) {
  state.activeTransform = TO::Move;

  liquid::editor::SetActiveTransform action(TO::Move);
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(SetActiveTransformActionTest,
       PredicateReturnsTrueIfActiveTransfromDoesNotEqualProvidedTransform) {
  state.activeTransform = TO::Move;

  liquid::editor::SetActiveTransform action(TO::Scale);
  EXPECT_TRUE(action.predicate(state));
}
