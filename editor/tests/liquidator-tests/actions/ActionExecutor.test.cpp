#include "liquid/core/Base.h"
#include "liquidator/actions/ActionExecutor.h"

#include "liquidator-tests/Testing.h"

class ActionExecutorTest : public ::testing::Test {
public:
  liquid::editor::WorkspaceState state;
  liquid::editor::ActionExecutor executor{state};
};

TEST_F(ActionExecutorTest, ExecuteFailsIfActionHasNoExecutor) {
  liquid::editor::Action TestAction{"TestAction"};

  EXPECT_DEATH(executor.execute(TestAction, liquid::String("Hello world")),
               ".*");
}

TEST_F(ActionExecutorTest, ExecuteCallsActionExecutorWithStateAndData) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  bool called = false;
  liquid::editor::Action TestAction{
      "TestAction", "",
      [&called](liquid::editor::WorkspaceState &state, std::any data) mutable {
        called = true;
        EXPECT_EQ(std::any_cast<liquid::String>(data), "Hello world");
        EXPECT_EQ(state.mode, liquid::editor::WorkspaceMode::Simulation);
        return liquid::editor::ActionExecutorResult{};
      }};

  executor.execute(TestAction, liquid::String("Hello world"));

  EXPECT_TRUE(called);
}
