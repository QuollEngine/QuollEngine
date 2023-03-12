#include "liquid/core/Base.h"
#include "liquidator/actions/EntityScriptingActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetScriptActionTest = ActionTestBase;

TEST_F(EntitySetScriptActionTest, ExecutorSetsScriptForEntityInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::EntitySetScript action(entity,
                                         liquid::LuaScriptAssetHandle{15});
  auto res = action.onExecute(state);

  EXPECT_EQ(state.scene.entityDatabase.get<liquid::Script>(entity).handle,
            liquid::LuaScriptAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetScriptActionTest,
       ExecutorSetsScriptForEntityInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::EntitySetScript action(entity,
                                         liquid::LuaScriptAssetHandle{15});
  auto res = action.onExecute(state);

  EXPECT_EQ(
      state.simulationScene.entityDatabase.get<liquid::Script>(entity).handle,
      liquid::LuaScriptAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}
