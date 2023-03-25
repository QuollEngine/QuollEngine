#include "liquid/core/Base.h"
#include "liquidator/actions/EditorGridActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using GridSetAxisLinesActionTest = ActionTestBase;

TEST_F(GridSetAxisLinesActionTest,
       ExecutorEnablesGridAxisLinesIfArgumentIsTrue) {
  state.grid.y = 0;

  liquid::editor::SetGridAxisLines action(true);
  action.onExecute(state);
  EXPECT_EQ(state.grid.y, 1);
}

TEST_F(GridSetAxisLinesActionTest,
       ExecutorDisablesGridAxisLinesIfArgumentIsFalse) {
  state.grid.y = 1;

  liquid::editor::SetGridAxisLines action(false);
  action.onExecute(state);
  EXPECT_EQ(state.grid.y, 0);
}

TEST_F(GridSetAxisLinesActionTest,
       PredicateReturnsFalseIfAxisLinesMatchProvidedArgument) {
  {
    state.grid.y = 0;
    liquid::editor::SetGridAxisLines action(false);
    EXPECT_FALSE(action.predicate(state));
  }

  {
    state.grid.y = 1;
    liquid::editor::SetGridAxisLines action(true);
    EXPECT_FALSE(action.predicate(state));
  }
}

TEST_F(GridSetAxisLinesActionTest,
       PredicateReturnsTrueIfAxisLinesDoNotMatchProvidedArguments) {
  {
    state.grid.y = 0;
    liquid::editor::SetGridAxisLines action(true);
    EXPECT_TRUE(action.predicate(state));
  }

  {
    state.grid.y = 1;
    liquid::editor::SetGridAxisLines action(false);
    EXPECT_TRUE(action.predicate(state));
  }
}

using GridSetLinesActionTest = ActionTestBase;

TEST_F(GridSetLinesActionTest, ExecutorEnablesGridLinesIfArgumentIsTrue) {
  state.grid.x = 0;

  liquid::editor::SetGridLines action(true);
  action.onExecute(state);
  EXPECT_EQ(state.grid.x, 1);
}

TEST_F(GridSetLinesActionTest, ExecutorDisablesGridLinesIfArgumentIsFalse) {
  state.grid.x = 1;

  liquid::editor::SetGridLines action(false);
  action.onExecute(state);
  EXPECT_EQ(state.grid.x, 0);
}

TEST_F(GridSetLinesActionTest,
       PredicateReturnsTrueIfGridLinesMatchProvidedArguments) {
  {
    state.grid.x = 0;
    liquid::editor::SetGridLines action(false);
    EXPECT_FALSE(action.predicate(state));
  }

  {
    state.grid.x = 1;
    liquid::editor::SetGridLines action(true);
    EXPECT_FALSE(action.predicate(state));
  }
}

TEST_F(GridSetLinesActionTest,
       PredicateReturnsTrueIfGridLinesDoNotMatchProvidedArguments) {
  {
    state.grid.x = 0;
    liquid::editor::SetGridLines action(false);
    EXPECT_FALSE(action.predicate(state));
  }

  {
    state.grid.x = 1;
    liquid::editor::SetGridLines action(true);
    EXPECT_FALSE(action.predicate(state));
  }
}
