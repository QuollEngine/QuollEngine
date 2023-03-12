#include "liquid/core/Base.h"
#include "liquidator/actions/DeleteEntityAction.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using DeleteEntityActionTest = ActionTestBase;

TEST_F(DeleteEntityActionTest,
       ExecuteAddsDeleteComponentToEntityInSceneIfWorkspaceModeIsEdit) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::DeleteEntityAction action(entity);
  auto res = action.onExecute(state);

  EXPECT_TRUE(state.scene.entityDatabase.has<liquid::Delete>(entity));
  ASSERT_EQ(res.entitiesToDelete.size(), 1);
  EXPECT_EQ(res.entitiesToDelete.at(0), entity);
}

TEST_F(
    DeleteEntityActionTest,
    ExecuteAddsDeleteComponentToEntityInSimulationSceneIfWorkspaceModeIsSimulation) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::DeleteEntityAction action(entity);
  auto res = action.onExecute(state);

  EXPECT_TRUE(state.simulationScene.entityDatabase.has<liquid::Delete>(entity));
  ASSERT_EQ(res.entitiesToDelete.size(), 1);
  EXPECT_EQ(res.entitiesToDelete.at(0), entity);
}

TEST_F(DeleteEntityActionTest,
       ExecuteSetsSelectedEntityToNullIfSelectedEntityIsProvidedEntity) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();
  state.selectedEntity = entity;

  liquid::editor::DeleteEntityAction action(entity);
  auto res = action.onExecute(state);

  EXPECT_EQ(state.selectedEntity, liquid::Entity::Null);
}

TEST_F(
    DeleteEntityActionTest,
    ExecuteDoesNotSetSelectedEntityToNullIfSelectedEntityIsNotProvidedEntity) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();
  state.selectedEntity = state.simulationScene.entityDatabase.create();

  liquid::editor::DeleteEntityAction action(entity);
  auto res = action.onExecute(state);

  EXPECT_NE(state.selectedEntity, liquid::Entity::Null);
}
