#include "liquid/core/Base.h"
#include "liquidator/actions/EntityCameraActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetPerspectiveLensActionTest = ActionTestBase;

TEST_F(EntitySetPerspectiveLensActionTest,
       ExecutorSetsPerspectiveLensComponentForEntityInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::EntitySetPerspectiveLens action(entity, {45.0f});
  auto res = action.onExecute(state);

  EXPECT_EQ(
      state.scene.entityDatabase.get<liquid::PerspectiveLens>(entity).fovY,
      45.0f);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetPerspectiveLensActionTest,
       ExecutorSetsPerspectiveLensComponentForEntityInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::EntitySetPerspectiveLens action(entity, {45.0f});
  auto res = action.onExecute(state);

  EXPECT_EQ(
      state.simulationScene.entityDatabase.get<liquid::PerspectiveLens>(entity)
          .fovY,
      45.0f);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

using SetEntityPerspectiveLensAutoAspectRatioTest = ActionTestBase;

TEST_F(SetEntityPerspectiveLensAutoAspectRatioTest,
       ExecutorSetPerspectiveLensAutoAspectRatioComponentInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::EntitySetCameraAutoAspectRatio action(entity);
  auto res = action.onExecute(state);

  EXPECT_TRUE(state.scene.entityDatabase.has<liquid::AutoAspectRatio>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(SetEntityPerspectiveLensAutoAspectRatioTest,
       ExecutorSetPerspectiveLensAutoAspectRatioComponentInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::EntitySetCameraAutoAspectRatio action(entity);
  auto res = action.onExecute(state);

  EXPECT_TRUE(state.simulationScene.entityDatabase.has<liquid::AutoAspectRatio>(
      entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(SetEntityPerspectiveLensAutoAspectRatioTest,
       PredicateReturnsTrueIfEntityHasNoAutoAspectRatioComponentScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::EntitySetCameraAutoAspectRatio action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_F(
    SetEntityPerspectiveLensAutoAspectRatioTest,
    PredicateReturnsTrueIfEntityHasNoAutoAspectRatioComponentInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::EntitySetCameraAutoAspectRatio action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_F(SetEntityPerspectiveLensAutoAspectRatioTest,
       PredicateReturnsFalseIfEntityHasAutoAspectRatioComponentScene) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::AutoAspectRatio>(entity, {});

  liquid::editor::EntitySetCameraAutoAspectRatio action(entity);
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(
    SetEntityPerspectiveLensAutoAspectRatioTest,
    PredicateReturnsFalseIfEntityHasAutoAspectRatioComponentInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();
  state.simulationScene.entityDatabase.set<liquid::AutoAspectRatio>(entity, {});

  liquid::editor::EntitySetCameraAutoAspectRatio action(entity);
  EXPECT_FALSE(action.predicate(state));
}

using SetEntityPerspectiveLensCustomAspectRatioTest = ActionTestBase;

TEST_F(SetEntityPerspectiveLensCustomAspectRatioTest,
       ExecutorRemovesAutoAspectRatioComponentForEntityInScene) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::AutoAspectRatio>(entity, {});

  liquid::editor::EntitySetCameraCustomAspectRatio action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(state.scene.entityDatabase.has<liquid::AutoAspectRatio>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(SetEntityPerspectiveLensCustomAspectRatioTest,
       ExecutorRemovesAutoAspectRatioComponentForEntityInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();
  state.simulationScene.entityDatabase.set<liquid::AutoAspectRatio>(entity, {});

  liquid::editor::EntitySetCameraCustomAspectRatio action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(
      state.simulationScene.entityDatabase.has<liquid::AutoAspectRatio>(
          entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(SetEntityPerspectiveLensCustomAspectRatioTest,
       PredicateReturnsTrueIfEntityHasAutoAspectRatioComponentInScene) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::AutoAspectRatio>(entity, {});

  liquid::editor::EntitySetCameraCustomAspectRatio action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_F(
    SetEntityPerspectiveLensCustomAspectRatioTest,
    PredicateReturnsTrueIfEntityHasAutoAspectRatioComponentInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();
  state.simulationScene.entityDatabase.set<liquid::AutoAspectRatio>(entity, {});

  liquid::editor::EntitySetCameraCustomAspectRatio action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_F(SetEntityPerspectiveLensCustomAspectRatioTest,
       PredicateReturnsFalseIfEntityHasNoAutoAspectRatioComponentInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::EntitySetCameraCustomAspectRatio action(entity);
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(
    SetEntityPerspectiveLensCustomAspectRatioTest,
    PredicateReturnsFalseIfEntityHasNoAutoAspectRatioComponentInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::EntitySetCameraCustomAspectRatio action(entity);
  EXPECT_FALSE(action.predicate(state));
}
