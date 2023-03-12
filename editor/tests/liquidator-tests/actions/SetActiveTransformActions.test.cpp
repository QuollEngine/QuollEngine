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
  EXPECT_EQ(state.activeTransform, TO::Rotate);

  liquid::editor::SetActiveTransformToMoveAction.onExecute(state, {});

  EXPECT_EQ(state.activeTransform, TO::Move);
}

TEST_F(SetActiveTransformToMoveActionTest,
       PredicateReturnsFalseIfActiveTransformIsNotMove) {
  state.activeTransform = TO::Rotate;
  EXPECT_FALSE(liquid::editor::SetActiveTransformToMoveAction.predicate(state));
}

TEST_F(SetActiveTransformToMoveActionTest,
       PredicateReturnsTrueIfActiveTransformIsMove) {
  state.activeTransform = TO::Move;
  EXPECT_TRUE(liquid::editor::SetActiveTransformToMoveAction.predicate(state));
}

using SetActiveTransformToRotateActionTest = SetActiveTransformActionsTestBase;

TEST_F(SetActiveTransformToRotateActionTest,
       ExecutorChangesActiveTransformToRotate) {
  EXPECT_EQ(state.activeTransform, TO::Move);

  liquid::editor::SetActiveTransformToRotateAction.onExecute(state, {});

  EXPECT_EQ(state.activeTransform, TO::Rotate);
}

TEST_F(SetActiveTransformToRotateActionTest,
       PredicateReturnsFalseIfActiveTransformIsNotRotate) {
  state.activeTransform = TO::Move;
  EXPECT_FALSE(
      liquid::editor::SetActiveTransformToRotateAction.predicate(state));
}

TEST_F(SetActiveTransformToRotateActionTest,
       PredicateReturnsTrueIfActiveTransformIsRotate) {
  state.activeTransform = TO::Rotate;
  EXPECT_TRUE(
      liquid::editor::SetActiveTransformToRotateAction.predicate(state));
}

using SetActiveTransformToScaleActionTest = SetActiveTransformActionsTestBase;

TEST_F(SetActiveTransformToScaleActionTest,
       ExecutorChangesActiveTransformToScale) {
  EXPECT_EQ(state.activeTransform, TO::Move);

  liquid::editor::SetActiveTransformToScaleAction.onExecute(state, {});

  EXPECT_EQ(state.activeTransform, TO::Scale);
}

TEST_F(SetActiveTransformToRotateActionTest,
       PredicateReturnsFalseIfActiveTransformIsNotScale) {
  state.activeTransform = TO::Move;
  EXPECT_FALSE(
      liquid::editor::SetActiveTransformToScaleAction.predicate(state));
}

TEST_F(SetActiveTransformToRotateActionTest,
       PredicateReturnsTrueIfActiveTransformIsScale) {
  state.activeTransform = TO::Scale;
  EXPECT_TRUE(liquid::editor::SetActiveTransformToScaleAction.predicate(state));
}
