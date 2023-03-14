#include "liquid/core/Base.h"
#include "liquidator/actions/SetGridDataActions.h"

#include "liquidator-tests/Testing.h"

class SetGridDataActionsTestBase : public ::testing::Test {
public:
  liquid::AssetRegistry registry;
  liquid::editor::WorkspaceState state{{}, registry};
};

using SetGridAxisLines = SetGridDataActionsTestBase;

TEST_F(SetGridAxisLines, ExecutorEnablesGridAxisLinesIfArgumentIsTrue) {
  state.grid.y = 0;

  liquid::editor::SetGridAxisLinesAction action(true);
  action.onExecute(state);
  EXPECT_EQ(state.grid.y, 1);
}

TEST_F(SetGridAxisLines, ExecutorDisablesGridAxisLinesIfArgumentIsFalse) {
  state.grid.y = 1;

  liquid::editor::SetGridAxisLinesAction action(false);
  action.onExecute(state);
  EXPECT_EQ(state.grid.y, 0);
}

TEST_F(SetGridAxisLines,
       PredicateReturnsFalseIfAxisLinesMatchProvidedArgument) {
  {
    state.grid.y = 0;
    liquid::editor::SetGridAxisLinesAction action(false);
    EXPECT_FALSE(action.predicate(state));
  }

  {
    state.grid.y = 1;
    liquid::editor::SetGridAxisLinesAction action(true);
    EXPECT_FALSE(action.predicate(state));
  }
}

TEST_F(SetGridAxisLines,
       PredicateReturnsTrueIfAxisLinesDoNotMatchProvidedArguments) {
  {
    state.grid.y = 0;
    liquid::editor::SetGridAxisLinesAction action(true);
    EXPECT_TRUE(action.predicate(state));
  }

  {
    state.grid.y = 1;
    liquid::editor::SetGridAxisLinesAction action(false);
    EXPECT_TRUE(action.predicate(state));
  }
}

using SetGridLines = SetGridDataActionsTestBase;

TEST_F(SetGridLines, ExecutorEnablesGridLinesIfArgumentIsTrue) {
  state.grid.x = 0;

  liquid::editor::SetGridLinesAction action(true);
  action.onExecute(state);
  EXPECT_EQ(state.grid.x, 1);
}

TEST_F(SetGridLines, ExecutorDisablesGridLinesIfArgumentIsFalse) {
  state.grid.x = 1;

  liquid::editor::SetGridLinesAction action(false);
  action.onExecute(state);
  EXPECT_EQ(state.grid.x, 0);
}

TEST_F(SetGridLines, PredicateReturnsTrueIfGridLinesMatchProvidedArguments) {
  {
    state.grid.x = 0;
    liquid::editor::SetGridLinesAction action(false);
    EXPECT_FALSE(action.predicate(state));
  }

  {
    state.grid.x = 1;
    liquid::editor::SetGridLinesAction action(true);
    EXPECT_FALSE(action.predicate(state));
  }
}

TEST_F(SetGridLines,
       PredicateReturnsTrueIfGridLinesDoNotMatchProvidedArguments) {
  {
    state.grid.x = 0;
    liquid::editor::SetGridLinesAction action(false);
    EXPECT_FALSE(action.predicate(state));
  }

  {
    state.grid.x = 1;
    liquid::editor::SetGridLinesAction action(true);
    EXPECT_FALSE(action.predicate(state));
  }
}
