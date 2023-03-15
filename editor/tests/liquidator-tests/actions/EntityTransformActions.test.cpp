#include "liquid/core/Base.h"
#include "liquidator/actions/EntityTransformActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetLocalTransformActionTest = ActionTestBase;

TEST_F(EntitySetLocalTransformActionTest,
       ExecutorSetsLocalTransformForEntityInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::EntitySetLocalTransform action(
      entity, liquid::LocalTransform{glm::vec3{2.5f}});

  auto res = action.onExecute(state);

  EXPECT_EQ(state.scene.entityDatabase.get<liquid::LocalTransform>(entity)
                .localPosition,
            glm::vec3(2.5f));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetLocalTransformActionTest,
       ExecutorSetsLocalTransformForEntityInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::EntitySetLocalTransform action(
      entity, liquid::LocalTransform{glm::vec3{2.5f}});

  auto res = action.onExecute(state);

  EXPECT_EQ(
      state.simulationScene.entityDatabase.get<liquid::LocalTransform>(entity)
          .localPosition,
      glm::vec3(2.5f));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}
