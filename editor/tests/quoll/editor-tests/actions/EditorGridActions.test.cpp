#include "quoll/core/Base.h"
#include "quoll/editor/actions/EditorGridActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

using GridSetAxisLinesActionTest = ActionTestBase;

TEST_F(GridSetAxisLinesActionTest,
       ExecutorEnablesGridAxisLinesIfArgumentIsTrue) {
  state.grid.y = 0;

  quoll::editor::SetGridAxisLines action(true);
  action.onExecute(state, assetRegistry);
  EXPECT_EQ(state.grid.y, 1);
}

TEST_F(GridSetAxisLinesActionTest,
       ExecutorDisablesGridAxisLinesIfArgumentIsFalse) {
  state.grid.y = 1;

  quoll::editor::SetGridAxisLines action(false);
  action.onExecute(state, assetRegistry);
  EXPECT_EQ(state.grid.y, 0);
}

TEST_F(GridSetAxisLinesActionTest,
       PredicateReturnsFalseIfAxisLinesMatchProvidedArgument) {
  {
    state.grid.y = 0;
    quoll::editor::SetGridAxisLines action(false);
    EXPECT_FALSE(action.predicate(state, assetRegistry));
  }

  {
    state.grid.y = 1;
    quoll::editor::SetGridAxisLines action(true);
    EXPECT_FALSE(action.predicate(state, assetRegistry));
  }
}

TEST_F(GridSetAxisLinesActionTest,
       PredicateReturnsTrueIfAxisLinesDoNotMatchProvidedArguments) {
  {
    state.grid.y = 0;
    quoll::editor::SetGridAxisLines action(true);
    EXPECT_TRUE(action.predicate(state, assetRegistry));
  }

  {
    state.grid.y = 1;
    quoll::editor::SetGridAxisLines action(false);
    EXPECT_TRUE(action.predicate(state, assetRegistry));
  }
}

using GridSetLinesActionTest = ActionTestBase;

TEST_F(GridSetLinesActionTest, ExecutorEnablesGridLinesIfArgumentIsTrue) {
  state.grid.x = 0;

  quoll::editor::SetGridLines action(true);
  action.onExecute(state, assetRegistry);
  EXPECT_EQ(state.grid.x, 1);
}

TEST_F(GridSetLinesActionTest, ExecutorDisablesGridLinesIfArgumentIsFalse) {
  state.grid.x = 1;

  quoll::editor::SetGridLines action(false);
  action.onExecute(state, assetRegistry);
  EXPECT_EQ(state.grid.x, 0);
}

TEST_F(GridSetLinesActionTest,
       PredicateReturnsTrueIfGridLinesMatchProvidedArguments) {
  {
    state.grid.x = 0;
    quoll::editor::SetGridLines action(false);
    EXPECT_FALSE(action.predicate(state, assetRegistry));
  }

  {
    state.grid.x = 1;
    quoll::editor::SetGridLines action(true);
    EXPECT_FALSE(action.predicate(state, assetRegistry));
  }
}

TEST_F(GridSetLinesActionTest,
       PredicateReturnsTrueIfGridLinesDoNotMatchProvidedArguments) {
  {
    state.grid.x = 0;
    quoll::editor::SetGridLines action(false);
    EXPECT_FALSE(action.predicate(state, assetRegistry));
  }

  {
    state.grid.x = 1;
    quoll::editor::SetGridLines action(true);
    EXPECT_FALSE(action.predicate(state, assetRegistry));
  }
}
