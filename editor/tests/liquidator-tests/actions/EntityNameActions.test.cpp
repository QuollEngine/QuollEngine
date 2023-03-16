#include "liquid/core/Base.h"
#include "liquidator/actions/EntityNameActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetNameActionTest = ActionTestBase;

TEST_P(EntitySetNameActionTest, ExecutorSetsEntityNameForEntity) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetName action(entity, {"Test name"});
  auto res = action.onExecute(state);

  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Name>(entity).name,
            "Test name");
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

InitActionsTestSuite(EntityActionsTest, EntitySetNameActionTest);
