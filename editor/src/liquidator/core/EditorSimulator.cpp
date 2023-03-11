#include "liquid/core/Base.h"
#include "EditorSimulator.h"

namespace liquid::editor {

EditorSimulator::EditorSimulator(EventSystem &eventSystem, Window &window,
                                 AssetRegistry &assetRegistry,
                                 EditorCamera &editorCamera)
    : mCameraAspectRatioUpdater(window),
      mScriptingSystem(eventSystem, assetRegistry),
      mAnimationSystem(assetRegistry), mPhysicsSystem(eventSystem),
      mEditorCamera(editorCamera), mAudioSystem(assetRegistry) {}

void EditorSimulator::update(float dt, WorkspaceState &state) {
  if (state.mode != mMode) {
    cleanupSimulationDatabase(state.simulationScene.entityDatabase);
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

void EditorSimulator::updateEditor(float dt, WorkspaceState &state) {
  auto &entityDatabase = state.scene.entityDatabase;
  mCameraAspectRatioUpdater.update(entityDatabase);
  mEditorCamera.update();

  mSkeletonUpdater.update(entityDatabase);
  mSceneUpdater.update(entityDatabase);

  mEntityDeleter.update(state.scene);
}

void EditorSimulator::updateSimulation(float dt, WorkspaceState &state) {
  auto &entityDatabase = state.simulationScene.entityDatabase;

  mCameraAspectRatioUpdater.update(entityDatabase);

  mScriptingSystem.start(entityDatabase);
  mScriptingSystem.update(dt, entityDatabase);
  mAnimationSystem.update(dt, entityDatabase);

  mSkeletonUpdater.update(entityDatabase);
  mSceneUpdater.update(entityDatabase);

  mPhysicsSystem.update(dt, entityDatabase);
  mAudioSystem.output(entityDatabase);

  mEntityDeleter.update(state.simulationScene);
}

} // namespace liquid::editor
