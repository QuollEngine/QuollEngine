#include "liquid/core/Base.h"
#include "liquidator/actions/EntityLightActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetDirectionalLightActionTest = ActionTestBase;

TEST_F(EntitySetDirectionalLightActionTest,
       SetsDirectionalLightToEntityInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::EntitySetDirectionalLight action(entity, {glm::vec4{0.3f}});
  auto res = action.onExecute(state);

  EXPECT_EQ(
      state.scene.entityDatabase.get<liquid::DirectionalLight>(entity).color,
      glm::vec4(0.3f));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetDirectionalLightActionTest,
       SetsDirectionalLightToEntityInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::EntitySetDirectionalLight action(entity, {glm::vec4{0.3f}});
  auto res = action.onExecute(state);

  EXPECT_EQ(
      state.simulationScene.entityDatabase.get<liquid::DirectionalLight>(entity)
          .color,
      glm::vec4(0.3f));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

using EntityEnableCascadedShadowMapActionTest = ActionTestBase;

TEST_F(EntityEnableCascadedShadowMapActionTest,
       ExecutorCreatesCascadedShadowMapComponentInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::EntityEnableCascadedShadowMap action(entity);
  auto res = action.onExecute(state);

  EXPECT_TRUE(
      state.scene.entityDatabase.has<liquid::CascadedShadowMap>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(
    EntityEnableCascadedShadowMapActionTest,
    PredicateReturnsTrueIfEntityDoesNotHaveCascadedShadowMapComponentInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::EntityEnableCascadedShadowMap action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_F(EntityEnableCascadedShadowMapActionTest,
       PredicateReturnsFalseIfEntityHasCascadedShadowMapComponentInScene) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::CascadedShadowMap>(entity, {});

  liquid::editor::EntityEnableCascadedShadowMap action(entity);
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(EntityEnableCascadedShadowMapActionTest,
       ExecutorCreatesCascadedShadowMapComponentInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::EntityEnableCascadedShadowMap action(entity);
  auto res = action.onExecute(state);

  EXPECT_TRUE(
      state.simulationScene.entityDatabase.has<liquid::CascadedShadowMap>(
          entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(
    EntityEnableCascadedShadowMapActionTest,
    PredicateReturnsTrueIfEntityDoesNotHaveCascadedShadowMapComponentInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::EntityEnableCascadedShadowMap action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_F(
    EntityEnableCascadedShadowMapActionTest,
    PredicateReturnsFalseIfEntityHasCascadedShadowMapComponentInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  auto entity = state.simulationScene.entityDatabase.create();
  state.simulationScene.entityDatabase.set<liquid::CascadedShadowMap>(entity,
                                                                      {});

  liquid::editor::EntityEnableCascadedShadowMap action(entity);
  EXPECT_FALSE(action.predicate(state));
}

using EntityDisableCascadedShadowMapActionTest = ActionTestBase;

TEST_F(EntityDisableCascadedShadowMapActionTest,
       ExecutorRemovesCascadedShadowMapComponentInScene) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::CascadedShadowMap>(entity, {});

  liquid::editor::EntityDisableCascadedShadowMap action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(
      state.scene.entityDatabase.has<liquid::CascadedShadowMap>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntityDisableCascadedShadowMapActionTest,
       PredicateReturnsTrueIfEntityHasCascadedShadowMapComponentInScene) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::CascadedShadowMap>(entity, {});

  liquid::editor::EntityDisableCascadedShadowMap action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_F(
    EntityDisableCascadedShadowMapActionTest,
    PredicateReturnsFalseIfEntityDoesNotHaveCascadedShadowMapComponentInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::EntityDisableCascadedShadowMap action(entity);
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(EntityDisableCascadedShadowMapActionTest,
       ExecutorRemovesCascadedShadowMapComponentInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();
  state.simulationScene.entityDatabase.set<liquid::CascadedShadowMap>(entity,
                                                                      {});

  liquid::editor::EntityDisableCascadedShadowMap action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(
      state.simulationScene.entityDatabase.has<liquid::CascadedShadowMap>(
          entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(
    EntityDisableCascadedShadowMapActionTest,
    PredicateReturnsTrueIfEntityHasCascadedShadowMapComponentInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  auto entity = state.simulationScene.entityDatabase.create();
  state.simulationScene.entityDatabase.set<liquid::CascadedShadowMap>(entity,
                                                                      {});

  liquid::editor::EntityDisableCascadedShadowMap action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_F(
    EntityDisableCascadedShadowMapActionTest,
    PredicateReturnsFalseIfEntityDoesNotHaveCascadedShadowMapComponentInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::EntityDisableCascadedShadowMap action(entity);
  EXPECT_FALSE(action.predicate(state));
}

using EntitySetCascadedShadowMapActionTest = ActionTestBase;

TEST_F(EntitySetCascadedShadowMapActionTest,
       ExecutorSetsCascadedShadowMapInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::EntitySetCascadedShadowMapAction action(entity, {0.4f});
  auto res = action.onExecute(state);

  EXPECT_EQ(state.scene.entityDatabase.get<liquid::CascadedShadowMap>(entity)
                .splitLambda,
            0.4f);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetCascadedShadowMapActionTest,
       ExecutorSetCascadedShadowMapInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::EntitySetCascadedShadowMapAction action(entity, {0.4f});
  auto res = action.onExecute(state);

  EXPECT_EQ(state.simulationScene.entityDatabase
                .get<liquid::CascadedShadowMap>(entity)
                .splitLambda,
            0.4f);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}
