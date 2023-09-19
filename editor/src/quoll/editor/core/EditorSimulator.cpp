#include "quoll/core/Base.h"
#include "EditorSimulator.h"

namespace quoll::editor {

EditorSimulator::EditorSimulator(InputDeviceManager &deviceManager,
                                 EventSystem &eventSystem, Window &window,
                                 AssetRegistry &assetRegistry,
                                 EditorCamera &editorCamera)
    : mInputMapSystem(deviceManager, assetRegistry),
      mCameraAspectRatioUpdater(window),
      mScriptingSystem(eventSystem, assetRegistry),
      mAnimationSystem(assetRegistry),
      mPhysicsSystem(PhysicsSystem::createPhysxBackend(eventSystem)),
      mEditorCamera(editorCamera), mAudioSystem(assetRegistry) {}

void EditorSimulator::update(float dt, WorkspaceState &state) {
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

void EditorSimulator::cleanupSimulationDatabase(
    EntityDatabase &simulationDatabase) {
  mPhysicsSystem.cleanup(simulationDatabase);
  mScriptingSystem.cleanup(simulationDatabase);
  mAudioSystem.cleanup(simulationDatabase);
}

void EditorSimulator::observeChanges(EntityDatabase &simulationDatabase) {
  mPhysicsSystem.observeChanges(simulationDatabase);
  mScriptingSystem.observeChanges(simulationDatabase);
  mAudioSystem.observeChanges(simulationDatabase);
}

void EditorSimulator::updateEditor(float dt, WorkspaceState &state) {
  auto &entityDatabase = state.scene.entityDatabase;
  mEntityDeleter.update(state.scene);

  mCameraAspectRatioUpdater.update(entityDatabase);
  mEditorCamera.update(state);

  mSkeletonUpdater.update(entityDatabase);
  mSceneUpdater.update(entityDatabase);
}

void EditorSimulator::updateSimulation(float dt, WorkspaceState &state) {
  auto &entityDatabase = state.simulationScene.entityDatabase;
  mEntityDeleter.update(state.simulationScene);

  mInputMapSystem.update(entityDatabase);

  mCameraAspectRatioUpdater.update(entityDatabase);

  mScriptingSystem.start(entityDatabase);
  mScriptingSystem.update(dt, entityDatabase);
  mAnimationSystem.update(dt, entityDatabase);

  mSkeletonUpdater.update(entityDatabase);
  mSceneUpdater.update(entityDatabase);

  mPhysicsSystem.update(dt, entityDatabase);
  mAudioSystem.output(entityDatabase);
}

} // namespace quoll::editor
