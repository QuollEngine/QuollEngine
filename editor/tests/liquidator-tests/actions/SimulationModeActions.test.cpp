#include "liquid/core/Base.h"
#include "liquidator/actions/SimulationModeActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using WM = liquid::editor::WorkspaceMode;

using StartSimulationModeActionTest = ActionTestBase;

TEST_F(StartSimulationModeActionTest, ExecutorSetsWorkspaceModeToSimulation) {
  state.mode = WM::Edit;

  liquid::editor::StartSimulationMode action;
  action.onExecute(state);
  EXPECT_EQ(state.mode, WM::Simulation);
}

TEST_F(StartSimulationModeActionTest,
       ExecutorDuplicatesSceneToSimulationScene) {
  auto activeCamera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::Camera>(activeCamera, {});

  state.mode = WM::Edit;
  state.scene.environment = liquid::Entity{12};
  state.scene.activeCamera = activeCamera;
  state.scene.dummyCamera = liquid::Entity{15};
  auto entity = state.scene.entityDatabase.create();

  EXPECT_EQ(state.simulationScene.environment, liquid::Entity::Null);
  EXPECT_EQ(state.simulationScene.activeCamera, liquid::Entity::Null);
  EXPECT_EQ(state.simulationScene.dummyCamera, liquid::Entity::Null);
  EXPECT_FALSE(state.simulationScene.entityDatabase.exists(entity));

  liquid::editor::StartSimulationMode action;
  action.onExecute(state);

  EXPECT_EQ(state.simulationScene.environment, state.scene.environment);
  EXPECT_EQ(state.simulationScene.activeCamera, state.scene.activeCamera);
  EXPECT_EQ(state.simulationScene.dummyCamera, state.scene.dummyCamera);
  EXPECT_EQ(state.activeCamera, state.scene.activeCamera);
  EXPECT_TRUE(state.simulationScene.entityDatabase.exists(entity));
}

TEST_F(StartSimulationModeActionTest,
       ExecutorSetsDummyCameraAsSceneCameraIfSceneHasNoActiveCamera) {
  auto dummyCamera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::Camera>(dummyCamera, {});

  state.mode = WM::Edit;
  state.scene.environment = liquid::Entity{12};
  state.scene.activeCamera = liquid::Entity{14};
  state.scene.dummyCamera = dummyCamera;
  auto entity = state.scene.entityDatabase.create();

  EXPECT_EQ(state.simulationScene.environment, liquid::Entity::Null);
  EXPECT_EQ(state.simulationScene.activeCamera, liquid::Entity::Null);
  EXPECT_EQ(state.simulationScene.dummyCamera, liquid::Entity::Null);
  EXPECT_FALSE(state.simulationScene.entityDatabase.exists(entity));

  liquid::editor::StartSimulationMode action;
  action.onExecute(state);

  EXPECT_EQ(state.simulationScene.environment, state.scene.environment);
  EXPECT_EQ(state.simulationScene.activeCamera, state.scene.dummyCamera);
  EXPECT_EQ(state.simulationScene.dummyCamera, state.scene.dummyCamera);
  EXPECT_EQ(state.activeCamera, state.scene.dummyCamera);
  EXPECT_TRUE(state.simulationScene.entityDatabase.exists(entity));
}

TEST_F(StartSimulationModeActionTest,
       PredicateReturnsTrueIfWorkspaceModeIsEdit) {
  state.mode = WM::Edit;

  liquid::editor::StartSimulationMode action;
  EXPECT_TRUE(action.predicate(state));
}

TEST_F(StartSimulationModeActionTest,
       PredicateReturnsFalseIfWorkspaceModeIsSimulation) {
  state.mode = WM::Simulation;

  liquid::editor::StartSimulationMode action;
  EXPECT_FALSE(action.predicate(state));
}

using StopSimulationModeActionTest = ActionTestBase;

TEST_F(StopSimulationModeActionTest, ExecutorSetsWorkspaceModeToSimulation) {
  state.mode = WM::Simulation;
  state.camera = state.scene.entityDatabase.create();

  liquid::editor::StopSimulationMode action;
  action.onExecute(state);
  EXPECT_EQ(state.mode, WM::Edit);
  EXPECT_EQ(state.activeCamera, state.camera);
}

TEST_F(StopSimulationModeActionTest,
       PredicateReturnsTrueIfWorkspaceModeIsSimulation) {
  state.mode = WM::Simulation;

  liquid::editor::StopSimulationMode action;
  EXPECT_TRUE(action.predicate(state));
}

TEST_F(StopSimulationModeActionTest,
       PredicateReturnsFalseIfWorkspaceModeIsSimulation) {
  state.mode = WM::Edit;

  liquid::editor::StopSimulationMode action;
  EXPECT_FALSE(action.predicate(state));
}
