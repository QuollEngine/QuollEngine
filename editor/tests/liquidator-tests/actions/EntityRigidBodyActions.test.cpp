#include "liquid/core/Base.h"
#include "liquidator/actions/EntityRigidBodyActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetRigidBodyActionTest = ActionTestBase;

TEST_F(EntitySetRigidBodyActionTest, ExecutorSetsTextForEntityInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::RigidBody rigidBody{};
  rigidBody.dynamicDesc.mass = 5.0f;

  liquid::editor::EntitySetRigidBody action(entity, rigidBody);
  auto res = action.onExecute(state);

  EXPECT_EQ(state.scene.entityDatabase.get<liquid::RigidBody>(entity)
                .dynamicDesc.mass,
            5.0f);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetRigidBodyActionTest,
       ExecutorSetsTextForEntityInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();

  liquid::RigidBody rigidBody{};
  rigidBody.dynamicDesc.mass = 5.0f;

  liquid::editor::EntitySetRigidBody action(entity, rigidBody);
  auto res = action.onExecute(state);

  EXPECT_EQ(state.simulationScene.entityDatabase.get<liquid::RigidBody>(entity)
                .dynamicDesc.mass,
            5.0f);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}
