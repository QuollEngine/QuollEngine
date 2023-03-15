#include "liquid/core/Base.h"
#include "liquidator/actions/EntityTextActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetTextActionTest = ActionTestBase;

TEST_F(EntitySetTextActionTest, ExecutorSetsTextForEntityInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::EntitySetText action(entity, liquid::Text{"Hello world"});
  auto res = action.onExecute(state);

  EXPECT_EQ(state.scene.entityDatabase.get<liquid::Text>(entity).text,
            "Hello world");
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetTextActionTest, ExecutorSetsTextForEntityInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::EntitySetText action(entity, liquid::Text{"Hello world"});
  auto res = action.onExecute(state);

  EXPECT_EQ(state.simulationScene.entityDatabase.get<liquid::Text>(entity).text,
            "Hello world");
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}
