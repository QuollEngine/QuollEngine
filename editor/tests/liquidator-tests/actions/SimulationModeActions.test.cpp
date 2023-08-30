#include "liquid/core/Base.h"
#include "liquidator/actions/SimulationModeActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using WM = quoll::editor::WorkspaceMode;

using StartSimulationModeActionTest = ActionTestBase;

TEST_F(StartSimulationModeActionTest, ExecutorSetsWorkspaceModeToSimulation) {
  state.mode = WM::Edit;

  quoll::editor::StartSimulationMode action;
  action.onExecute(state, assetRegistry);
  EXPECT_EQ(state.mode, WM::Simulation);
}

TEST_F(StartSimulationModeActionTest,
       ExecutorDuplicatesSceneToSimulationScene) {
  auto activeCamera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(activeCamera, {});

  state.mode = WM::Edit;
  state.scene.activeEnvironment = quoll::Entity{12};
  state.scene.activeCamera = activeCamera;
  state.scene.dummyCamera = quoll::Entity{15};
  auto entity = state.scene.entityDatabase.create();

  EXPECT_EQ(state.simulationScene.activeEnvironment, quoll::Entity::Null);
  EXPECT_EQ(state.simulationScene.dummyEnvironment, quoll::Entity::Null);
  EXPECT_EQ(state.simulationScene.activeCamera, quoll::Entity::Null);
  EXPECT_EQ(state.simulationScene.dummyCamera, quoll::Entity::Null);
  EXPECT_FALSE(state.simulationScene.entityDatabase.exists(entity));

  quoll::editor::StartSimulationMode action;
  action.onExecute(state, assetRegistry);

  EXPECT_EQ(state.simulationScene.activeEnvironment,
            state.scene.activeEnvironment);
  EXPECT_EQ(state.simulationScene.dummyEnvironment,
            state.scene.dummyEnvironment);
  EXPECT_EQ(state.simulationScene.activeCamera, state.scene.activeCamera);
  EXPECT_EQ(state.simulationScene.dummyCamera, state.scene.dummyCamera);
  EXPECT_EQ(state.activeCamera, state.scene.activeCamera);
  EXPECT_TRUE(state.simulationScene.entityDatabase.exists(entity));
}

TEST_F(StartSimulationModeActionTest,
       ExecutorSetsDummyCameraAsSceneCameraIfSceneHasNoActiveCamera) {
  auto dummyCamera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(dummyCamera, {});

  state.mode = WM::Edit;
  state.scene.activeEnvironment = quoll::Entity{12};
  state.scene.activeCamera = quoll::Entity{14};
  state.scene.dummyCamera = dummyCamera;
  auto entity = state.scene.entityDatabase.create();

  EXPECT_EQ(state.simulationScene.activeEnvironment, quoll::Entity::Null);
  EXPECT_EQ(state.simulationScene.activeCamera, quoll::Entity::Null);
  EXPECT_EQ(state.simulationScene.dummyCamera, quoll::Entity::Null);
  EXPECT_FALSE(state.simulationScene.entityDatabase.exists(entity));

  quoll::editor::StartSimulationMode action;
  action.onExecute(state, assetRegistry);

  EXPECT_EQ(state.simulationScene.activeEnvironment,
            state.scene.activeEnvironment);
  EXPECT_EQ(state.simulationScene.activeCamera, state.scene.dummyCamera);
  EXPECT_EQ(state.simulationScene.dummyCamera, state.scene.dummyCamera);
  EXPECT_EQ(state.activeCamera, state.scene.dummyCamera);
  EXPECT_TRUE(state.simulationScene.entityDatabase.exists(entity));
}

TEST_F(StartSimulationModeActionTest,
       PredicateReturnsTrueIfWorkspaceModeIsEdit) {
  state.mode = WM::Edit;

  quoll::editor::StartSimulationMode action;
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

TEST_F(StartSimulationModeActionTest,
       PredicateReturnsFalseIfWorkspaceModeIsSimulation) {
  state.mode = WM::Simulation;

  quoll::editor::StartSimulationMode action;
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

using StopSimulationModeActionTest = ActionTestBase;

TEST_F(StopSimulationModeActionTest, ExecutorSetsWorkspaceModeToSimulation) {
  state.mode = WM::Simulation;
  state.camera = state.scene.entityDatabase.create();

  quoll::editor::StopSimulationMode action;
  action.onExecute(state, assetRegistry);
  EXPECT_EQ(state.mode, WM::Edit);
  EXPECT_EQ(state.activeCamera, state.camera);
}

TEST_F(StopSimulationModeActionTest,
       ExecutorSetsSelectedEntityToNullIfEntityDoesNotExistInScene) {
  state.mode = WM::Simulation;
  state.camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.duplicate(state.simulationScene.entityDatabase);

  state.selectedEntity = state.simulationScene.entityDatabase.create();

  quoll::editor::StopSimulationMode action;
  action.onExecute(state, assetRegistry);
  EXPECT_EQ(state.mode, WM::Edit);
  EXPECT_EQ(state.activeCamera, state.camera);
  EXPECT_EQ(state.selectedEntity, quoll::Entity::Null);
}

TEST_F(
    StopSimulationModeActionTest,
    ExecutorSetsSelectedEntityToNullIfIfEntityIsADescendantOfEntityThatDoesNotExistInScene) {
  state.mode = WM::Simulation;
  state.camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.duplicate(state.simulationScene.entityDatabase);

  auto entity = state.simulationScene.entityDatabase.create();

  {
    auto e1 = state.simulationScene.entityDatabase.create();
    activeScene().entityDatabase.set<quoll::Parent>(e1, {entity});

    auto e2 = state.simulationScene.entityDatabase.create();
    activeScene().entityDatabase.set<quoll::Parent>(e2, {e1});
    state.selectedEntity = e2;
  }

  quoll::editor::StopSimulationMode action;
  action.onExecute(state, assetRegistry);
  EXPECT_EQ(state.mode, WM::Edit);
  EXPECT_EQ(state.activeCamera, state.camera);
  EXPECT_EQ(state.selectedEntity, quoll::Entity::Null);
}

TEST_F(StopSimulationModeActionTest,
       ExecutorDoesNotSetSelectedEntityToNullIfSelectedEntityExistsInScene) {
  state.mode = WM::Simulation;
  state.camera = state.scene.entityDatabase.create();
  state.selectedEntity = state.scene.entityDatabase.create();

  state.scene.entityDatabase.duplicate(state.simulationScene.entityDatabase);

  quoll::editor::StopSimulationMode action;
  action.onExecute(state, assetRegistry);
  EXPECT_EQ(state.mode, WM::Edit);
  EXPECT_EQ(state.activeCamera, state.camera);
  EXPECT_NE(state.selectedEntity, quoll::Entity::Null);
}

TEST_F(StopSimulationModeActionTest,
       PredicateReturnsTrueIfWorkspaceModeIsSimulation) {
  state.mode = WM::Simulation;

  quoll::editor::StopSimulationMode action;
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

TEST_F(StopSimulationModeActionTest,
       PredicateReturnsFalseIfWorkspaceModeIsSimulation) {
  state.mode = WM::Edit;

  quoll::editor::StopSimulationMode action;
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}
