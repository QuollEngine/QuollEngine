#include "liquid/core/Base.h"
#include "liquidator/actions/EntityNameActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetNameActionTest = ActionTestBase;

TEST_F(EntitySetNameActionTest, SetsEntityNameToProvidedValueInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::EntitySetName action(entity, {"Test name"});
  auto res = action.onExecute(state);

  EXPECT_EQ(state.scene.entityDatabase.get<liquid::Name>(entity).name,
            "Test name");
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetNameActionTest,
       SetsEntityNameToProvidedValueInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::EntitySetName action(entity, {"Test name"});
  auto res = action.onExecute(state);

  EXPECT_EQ(state.simulationScene.entityDatabase.get<liquid::Name>(entity).name,
            "Test name");
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}
