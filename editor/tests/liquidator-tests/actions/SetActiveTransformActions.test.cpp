#include "liquid/core/Base.h"
#include "liquidator/actions/SetActiveTransformActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using TO = liquid::editor::TransformOperation;

using SetActiveTransformActionTest = ActionTestBase;

TEST_F(SetActiveTransformActionTest,
       ExecutorChangesActiveTransformToProvidedTransform) {
  state.activeTransform = TO::Rotate;

  liquid::editor::SetActiveTransformAction action(TO::Move);
  action.onExecute(state);
  EXPECT_EQ(state.activeTransform, TO::Move);
}

TEST_F(SetActiveTransformActionTest,
       PredicateReturnsFalseIfActiveTransformEqualsProvidedTransform) {
  state.activeTransform = TO::Move;

  liquid::editor::SetActiveTransformAction action(TO::Move);
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(SetActiveTransformActionTest,
       PredicateReturnsTrueIfActiveTransfromDoesNotEqualProvidedTransform) {
  state.activeTransform = TO::Move;

  liquid::editor::SetActiveTransformAction action(TO::Scale);
  EXPECT_TRUE(action.predicate(state));
}
