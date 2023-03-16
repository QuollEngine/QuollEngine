#include "liquid/core/Base.h"
#include "liquidator/actions/EntityTextActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetTextActionTest = ActionTestBase;

TEST_P(EntitySetTextActionTest, ExecutorSetsTextForEntity) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetText action(entity, liquid::Text{"Hello world"});
  auto res = action.onExecute(state);

  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Text>(entity).text,
            "Hello world");
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

InitActionsTestSuite(EntityActionsTest, EntitySetTextActionTest);
