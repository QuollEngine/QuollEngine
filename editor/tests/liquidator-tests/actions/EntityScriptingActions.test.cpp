#include "liquid/core/Base.h"
#include "liquidator/actions/EntityScriptingActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetScriptActionTest = ActionTestBase;

TEST_P(EntitySetScriptActionTest, ExecutorSetsScriptForEntity) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetScript action(entity,
                                         liquid::LuaScriptAssetHandle{15});
  auto res = action.onExecute(state);

  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Script>(entity).handle,
            liquid::LuaScriptAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

InitActionsTestSuite(EntityActionsTest, EntitySetScriptActionTest);
