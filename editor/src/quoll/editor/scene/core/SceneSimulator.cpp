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
  auto &entityDatabase = state.scene.entityDatabase;
  mEntityDeleter.update(state.scene);

  mSkeletonUpdater.update(entityDatabase);
  mSceneUpdater.update(entityDatabase);
  mPhysicsSystem.update(dt, entityDatabase);

  mInputMapSystem.update(entityDatabase);

  mCameraAspectRatioUpdater.update(entityDatabase);

  mScriptingSystem.start(entityDatabase, mPhysicsSystem, mWindow.getSignals());
  mScriptingSystem.update(dt, entityDatabase);
  mAnimationSystem.update(dt, entityDatabase);

  mAudioSystem.output(entityDatabase);
}

} // namespace quoll::editor
