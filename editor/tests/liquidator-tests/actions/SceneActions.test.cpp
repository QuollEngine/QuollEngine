#include "liquid/core/Base.h"
#include "liquidator/actions/SceneActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using SceneSetStartingCameraActionTest = ActionTestBase;

TEST_F(SceneSetStartingCameraActionTest,
       ExecutorSetsSceneDataAndReturnsSaveScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::SceneSetStartingCamera action(entity);
  auto res = action.onExecute(state);

  EXPECT_EQ(state.scene.activeCamera, entity);
  EXPECT_TRUE(res.saveScene);
}

TEST_F(SceneSetStartingCameraActionTest,
       ExecutorSetsSimulationSceneDataAndReturnsSaveScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::SceneSetStartingCamera action(entity);
  auto res = action.onExecute(state);

  EXPECT_EQ(state.simulationScene.activeCamera, entity);
  EXPECT_TRUE(res.saveScene);
}

TEST_F(SceneSetStartingCameraActionTest,
       PredicateReturnsFalseIfProvidedEntityEqualsActiveCameraInScene) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  state.scene.activeCamera = entity;

  liquid::editor::SceneSetStartingCamera action(entity);
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(
    SceneSetStartingCameraActionTest,
    PredicateReturnsFalseIfProvidedEntityEqualsActiveCameraInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();
  state.simulationScene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  state.simulationScene.activeCamera = entity;

  liquid::editor::SceneSetStartingCamera action(entity);
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(SceneSetStartingCameraActionTest,
       PredicateReturnsFalseIfProvidedEntityDoesNotHavePerspectiveLensInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::SceneSetStartingCamera action(entity);
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(
    SceneSetStartingCameraActionTest,
    PredicateReturnsFalseIfProvidedEntityDoesNotHavePerspectiveLensInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::SceneSetStartingCamera action(entity);
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(
    SceneSetStartingCameraActionTest,
    PredicateReturnsTrueIfProvidedCameraEntityIsDifferentThanActiveCameraInScene) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});

  liquid::editor::SceneSetStartingCamera action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_F(
    SceneSetStartingCameraActionTest,
    PredicateReturnsTrueIfProvidedCameraEntityIsDifferentThanActiveCameraInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();
  state.simulationScene.entityDatabase.set<liquid::PerspectiveLens>(entity, {});

  liquid::editor::SceneSetStartingCamera action(entity);
  EXPECT_TRUE(action.predicate(state));
}
