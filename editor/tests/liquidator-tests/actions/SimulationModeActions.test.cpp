#include "liquid/core/Base.h"
#include "liquidator/actions/SimulationModeActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using WM = liquid::editor::WorkspaceMode;

using StartSimulationModeActionTest = ActionTestBase;

TEST_F(StartSimulationModeActionTest, ExecutorSetsWorkspaceModeToSimulation) {
  state.mode = WM::Edit;

  liquid::editor::StartSimulationMode action;
  action.onExecute(state, assetRegistry);
  EXPECT_EQ(state.mode, WM::Simulation);
}

TEST_F(StartSimulationModeActionTest,
       ExecutorDuplicatesSceneToSimulationScene) {
  auto activeCamera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::Camera>(activeCamera, {});

  state.mode = WM::Edit;
  state.scene.activeEnvironment = liquid::Entity{12};
  state.scene.activeCamera = activeCamera;
  state.scene.dummyCamera = liquid::Entity{15};
  auto entity = state.scene.entityDatabase.create();

  EXPECT_EQ(state.simulationScene.activeEnvironment, liquid::Entity::Null);
  EXPECT_EQ(state.simulationScene.dummyEnvironment, liquid::Entity::Null);
  EXPECT_EQ(state.simulationScene.activeCamera, liquid::Entity::Null);
  EXPECT_EQ(state.simulationScene.dummyCamera, liquid::Entity::Null);
  EXPECT_FALSE(state.simulationScene.entityDatabase.exists(entity));

  liquid::editor::StartSimulationMode action;
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
  state.scene.entityDatabase.set<liquid::Camera>(dummyCamera, {});

  state.mode = WM::Edit;
  state.scene.activeEnvironment = liquid::Entity{12};
  state.scene.activeCamera = liquid::Entity{14};
  state.scene.dummyCamera = dummyCamera;
  auto entity = state.scene.entityDatabase.create();

  EXPECT_EQ(state.simulationScene.activeEnvironment, liquid::Entity::Null);
  EXPECT_EQ(state.simulationScene.activeCamera, liquid::Entity::Null);
  EXPECT_EQ(state.simulationScene.dummyCamera, liquid::Entity::Null);
  EXPECT_FALSE(state.simulationScene.entityDatabase.exists(entity));

  liquid::editor::StartSimulationMode action;
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

  liquid::editor::StartSimulationMode action;
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

TEST_F(StartSimulationModeActionTest,
       PredicateReturnsFalseIfWorkspaceModeIsSimulation) {
  state.mode = WM::Simulation;

  liquid::editor::StartSimulationMode action;
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

using StopSimulationModeActionTest = ActionTestBase;

TEST_F(StopSimulationModeActionTest, ExecutorSetsWorkspaceModeToSimulation) {
  state.mode = WM::Simulation;
  state.camera = state.scene.entityDatabase.create();

  liquid::editor::StopSimulationMode action;
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

  liquid::editor::StopSimulationMode action;
  action.onExecute(state, assetRegistry);
  EXPECT_EQ(state.mode, WM::Edit);
  EXPECT_EQ(state.activeCamera, state.camera);
  EXPECT_EQ(state.selectedEntity, liquid::Entity::Null);
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
    activeScene().entityDatabase.set<liquid::Parent>(e1, {entity});

    auto e2 = state.simulationScene.entityDatabase.create();
    activeScene().entityDatabase.set<liquid::Parent>(e2, {e1});
    state.selectedEntity = e2;
  }

  liquid::editor::StopSimulationMode action;
  action.onExecute(state, assetRegistry);
  EXPECT_EQ(state.mode, WM::Edit);
  EXPECT_EQ(state.activeCamera, state.camera);
  EXPECT_EQ(state.selectedEntity, liquid::Entity::Null);
}

TEST_F(StopSimulationModeActionTest,
       ExecutorDoesNotSetSelectedEntityToNullIfSelectedEntityExistsInScene) {
  state.mode = WM::Simulation;
  state.camera = state.scene.entityDatabase.create();
  state.selectedEntity = state.scene.entityDatabase.create();

  state.scene.entityDatabase.duplicate(state.simulationScene.entityDatabase);

  liquid::editor::StopSimulationMode action;
  action.onExecute(state, assetRegistry);
  EXPECT_EQ(state.mode, WM::Edit);
  EXPECT_EQ(state.activeCamera, state.camera);
  EXPECT_NE(state.selectedEntity, liquid::Entity::Null);
}

TEST_F(StopSimulationModeActionTest,
       PredicateReturnsTrueIfWorkspaceModeIsSimulation) {
  state.mode = WM::Simulation;

  liquid::editor::StopSimulationMode action;
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

TEST_F(StopSimulationModeActionTest,
       PredicateReturnsFalseIfWorkspaceModeIsSimulation) {
  state.mode = WM::Edit;

  liquid::editor::StopSimulationMode action;
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}
