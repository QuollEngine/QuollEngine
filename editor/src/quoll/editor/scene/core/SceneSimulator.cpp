#include "quoll/core/Base.h"
#include "SceneSimulator.h"

namespace quoll::editor {

SceneSimulator::SceneSimulator(InputDeviceManager &deviceManager,
                               Window &window, AssetRegistry &assetRegistry,
                               EditorCamera &editorCamera)
    : mWindow(window), mInputMapSystem(deviceManager, assetRegistry),
      mScriptingSystem(assetRegistry), mAnimationSystem(assetRegistry),
      mPhysicsSystem(PhysicsSystem::createPhysxBackend()),
      mEditorCamera(editorCamera), mAudioSystem(assetRegistry),
      mAssetRegistry(assetRegistry) {}

void SceneSimulator::update(f32 dt, WorkspaceState &state) {
  if (state.mode != mMode) {
    // Reobserve changes when switching from
    // edit to simulation mode
    if (mMode == WorkspaceMode::Edit) {
      observeChanges(state.simulationScene.entityDatabase);
    }

    // Cleanup simulation database when switchin from simulation
    // to edit mode
    if (mMode == WorkspaceMode::Simulation) {
      cleanupSimulationDatabase(state.simulationScene.entityDatabase);
    }
    mMode = state.mode;
  }

  if (state.mode == WorkspaceMode::Edit) {
    updateEditor(dt, state);
  } else {
    updateSimulation(dt, state);
  }
}

void SceneSimulator::render(EntityDatabase &db) {
  mUICanvasUpdater.render(db, mAssetRegistry);
}

void SceneSimulator::cleanupSimulationDatabase(
    EntityDatabase &simulationDatabase) {
  mPhysicsSystem.cleanup(simulationDatabase);
  mScriptingSystem.cleanup(simulationDatabase);
  mAudioSystem.cleanup(simulationDatabase);
}

void SceneSimulator::observeChanges(EntityDatabase &simulationDatabase) {
  mPhysicsSystem.observeChanges(simulationDatabase);
  mScriptingSystem.observeChanges(simulationDatabase);
  mAudioSystem.observeChanges(simulationDatabase);
}

void SceneSimulator::updateEditor(f32 dt, WorkspaceState &state) {
  auto &entityDatabase = state.scene.entityDatabase;
  mEntityDeleter.update(state.scene);

  mCameraAspectRatioUpdater.update(entityDatabase);
  mEditorCamera.update(state);

  mSkeletonUpdater.update(entityDatabase);
  mSceneUpdater.update(entityDatabase);
}

void SceneSimulator::updateSimulation(f32 dt, WorkspaceState &state) {
  auto &entityDatabase = state.simulationScene.entityDatabase;
  mEntityDeleter.update(state.simulationScene);

  mInputMapSystem.update(entityDatabase);

  mCameraAspectRatioUpdater.update(entityDatabase);
  mPhysicsSystem.update(dt, entityDatabase);

  mScriptingSystem.start(entityDatabase, mPhysicsSystem, mWindow.getSignals());
  mScriptingSystem.update(dt, entityDatabase);
  mAnimationSystem.update(dt, entityDatabase);

  mSkeletonUpdater.update(entityDatabase);
  mSceneUpdater.update(entityDatabase);

  mAudioSystem.output(entityDatabase);
}

} // namespace quoll::editor
