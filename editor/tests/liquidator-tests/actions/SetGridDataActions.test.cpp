#include "liquid/core/Base.h"
#include "liquidator/actions/SetGridDataActions.h"

#include "liquidator-tests/Testing.h"

class SetGridDataActionsTestBase : public ::testing::Test {
public:
  liquid::AssetRegistry registry;
  liquid::editor::WorkspaceState state{registry};
};

using SetGridAxisLines = SetGridDataActionsTestBase;

TEST_F(SetGridAxisLines, ExecutorEnablesGridAxisLinesIfArgumentIsTrue) {
  EXPECT_EQ(state.grid.y, 0);
  liquid::editor::SetGridAxisLinesAction.onExecute(state, true);
  EXPECT_EQ(state.grid.y, 1);
}

TEST_F(SetGridAxisLines, ExecutorDisablesGridAxisLinesIfArgumentIsFalse) {
  state.grid.y = 1;
  EXPECT_EQ(state.grid.y, 1);
  liquid::editor::SetGridAxisLinesAction.onExecute(state, false);
  EXPECT_EQ(state.grid.y, 0);
}

TEST_F(SetGridAxisLines, PredicateReturnsFalseIfAxisLinesAreDisabled) {
  liquid::editor::SetGridAxisLinesAction.onExecute(state, false);
  EXPECT_FALSE(liquid::editor::SetGridAxisLinesAction.predicate(state));
}

TEST_F(SetGridAxisLines, PredicateReturnsFalseIfAxisLinesAreEnabled) {
  liquid::editor::SetGridAxisLinesAction.onExecute(state, true);
  EXPECT_TRUE(liquid::editor::SetGridAxisLinesAction.predicate(state));
}

using SetGridines = SetGridDataActionsTestBase;

TEST_F(SetGridAxisLines, ExecutorEnablesGridLinesIfArgumentIsTrue) {
  EXPECT_EQ(state.grid.x, 0);
  liquid::editor::SetGridLinesAction.onExecute(state, true);
  EXPECT_EQ(state.grid.x, 1);
}

TEST_F(SetGridAxisLines, ExecutorDisablesGridLinesIfArgumentIsFalse) {
  state.grid.x = 1;
  EXPECT_EQ(state.grid.x, 1);
  liquid::editor::SetGridLinesAction.onExecute(state, false);
  EXPECT_EQ(state.grid.x, 0);
}

TEST_F(SetGridAxisLines, PredicateReturnsFalseIfGridLinesAreDisabled) {
  liquid::editor::SetGridLinesAction.onExecute(state, false);
  EXPECT_FALSE(liquid::editor::SetGridLinesAction.predicate(state));
}

TEST_F(SetGridAxisLines, PredicateReturnsFalseIfGridLinesAreEnabled) {
  liquid::editor::SetGridLinesAction.onExecute(state, true);
  EXPECT_TRUE(liquid::editor::SetGridLinesAction.predicate(state));
}
