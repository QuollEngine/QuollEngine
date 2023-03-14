#include "liquid/core/Base.h"
#include "liquidator/actions/SetActiveTransformActions.h"

#include "liquidator-tests/Testing.h"

using TO = liquid::editor::TransformOperation;

class SetActiveTransformActionsTestBase : public ::testing::Test {
public:
  liquid::AssetRegistry registry;
  liquid::editor::WorkspaceState state{{}, registry};
};

using SetActiveTransformToMoveActionTest = SetActiveTransformActionsTestBase;

TEST_F(SetActiveTransformToMoveActionTest,
       ExecutorChangesActiveTransformToMove) {
  state.activeTransform = TO::Rotate;

  liquid::editor::SetActiveTransformToMoveAction action;
  action.onExecute(state);
  EXPECT_EQ(state.activeTransform, TO::Move);
}

TEST_F(SetActiveTransformToMoveActionTest,
       PredicateReturnsFalseIfActiveTransformIsNotMove) {
  state.activeTransform = TO::Rotate;

  liquid::editor::SetActiveTransformToMoveAction action;
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(SetActiveTransformToMoveActionTest,
       PredicateReturnsTrueIfActiveTransformIsMove) {
  state.activeTransform = TO::Move;

  liquid::editor::SetActiveTransformToMoveAction action;
  EXPECT_TRUE(action.predicate(state));
}

using SetActiveTransformToRotateActionTest = SetActiveTransformActionsTestBase;

TEST_F(SetActiveTransformToRotateActionTest,
       ExecutorChangesActiveTransformToRotate) {
  EXPECT_EQ(state.activeTransform, TO::Move);

  liquid::editor::SetActiveTransformToRotateAction action;
  action.onExecute(state);
  EXPECT_EQ(state.activeTransform, TO::Rotate);
}

TEST_F(SetActiveTransformToRotateActionTest,
       PredicateReturnsFalseIfActiveTransformIsNotRotate) {
  state.activeTransform = TO::Move;

  liquid::editor::SetActiveTransformToRotateAction action;
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(SetActiveTransformToRotateActionTest,
       PredicateReturnsTrueIfActiveTransformIsRotate) {
  state.activeTransform = TO::Rotate;

  liquid::editor::SetActiveTransformToRotateAction action;
  EXPECT_TRUE(action.predicate(state));
}

using SetActiveTransformToScaleActionTest = SetActiveTransformActionsTestBase;

TEST_F(SetActiveTransformToScaleActionTest,
       ExecutorChangesActiveTransformToScale) {
  EXPECT_EQ(state.activeTransform, TO::Move);

  liquid::editor::SetActiveTransformToScaleAction action;
  action.onExecute(state);
  EXPECT_EQ(state.activeTransform, TO::Scale);
}

TEST_F(SetActiveTransformToRotateActionTest,
       PredicateReturnsFalseIfActiveTransformIsNotScale) {
  state.activeTransform = TO::Move;

  liquid::editor::SetActiveTransformToScaleAction action;
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(SetActiveTransformToRotateActionTest,
       PredicateReturnsTrueIfActiveTransformIsScale) {
  state.activeTransform = TO::Scale;

  liquid::editor::SetActiveTransformToScaleAction action;
  EXPECT_TRUE(action.predicate(state));
}
