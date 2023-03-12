#include "liquid/core/Base.h"
#include "liquidator/actions/DeleteEntityAction.h"

#include "liquidator-tests/Testing.h"

class DeleteEntityActionTest : public ::testing::Test {
public:
  liquid::AssetRegistry registry;
  liquid::editor::WorkspaceState state{{}, registry};
};

TEST_F(DeleteEntityActionTest, ExecuteFailsIfProvidedArgumentIsNotEntity) {
  EXPECT_THROW(liquid::editor::DeleteEntityAction.onExecute(state, "test"),
               std::bad_any_cast);
}

TEST_F(DeleteEntityActionTest,
       ExecuteAddsDeleteComponentToEntityInSceneIfWorkspaceModeIsEdit) {
  auto entity = state.scene.entityDatabase.create();

  auto res = liquid::editor::DeleteEntityAction.onExecute(state, entity);

  EXPECT_TRUE(state.scene.entityDatabase.has<liquid::Delete>(entity));
  ASSERT_EQ(res.entitiesToDelete.size(), 1);
  EXPECT_EQ(res.entitiesToDelete.at(0), entity);
}

TEST_F(
    DeleteEntityActionTest,
    ExecuteAddsDeleteComponentToEntityInSimulationSceneIfWorkspaceModeIsSimulation) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();

  auto res = liquid::editor::DeleteEntityAction.onExecute(state, entity);

  EXPECT_TRUE(state.simulationScene.entityDatabase.has<liquid::Delete>(entity));
  ASSERT_EQ(res.entitiesToDelete.size(), 1);
  EXPECT_EQ(res.entitiesToDelete.at(0), entity);
}

TEST_F(DeleteEntityActionTest,
       ExecuteSetsSelectedEntityToNullIfSelectedEntityIsProvidedEntity) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();
  state.selectedEntity = entity;

  auto res = liquid::editor::DeleteEntityAction.onExecute(state, entity);

  EXPECT_EQ(state.selectedEntity, liquid::Entity::Null);
}

TEST_F(
    DeleteEntityActionTest,
    ExecuteDoesNotSetSelectedEntityToNullIfSelectedEntityIsNotProvidedEntity) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();
  state.selectedEntity = state.simulationScene.entityDatabase.create();

  auto res = liquid::editor::DeleteEntityAction.onExecute(state, entity);

  EXPECT_NE(state.selectedEntity, liquid::Entity::Null);
}
