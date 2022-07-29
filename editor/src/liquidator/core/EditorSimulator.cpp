#include "liquid/core/Base.h"
#include "EditorSimulator.h"

namespace liquidator {

EditorSimulator::EditorSimulator(liquid::EventSystem &eventSystem,
                                 liquid::Window &window,
                                 liquid::AssetRegistry &assetRegistry,
                                 EditorCamera &editorCamera)
    : mCameraAspectRatioUpdater(window),
      mScriptingSystem(eventSystem, assetRegistry),
      mAnimationSystem(assetRegistry), mPhysicsSystem(eventSystem),
      mEditorCamera(editorCamera), mAudioSystem(assetRegistry) {
  useEditorUpdate();
}

void EditorSimulator::update(float dt, liquid::EntityDatabase &entityDatabase) {
  mUpdater(dt, entityDatabase);
}

void EditorSimulator::cleanupSimulationDatabase(
    liquid::EntityDatabase &simulationDatabase) {
  mPhysicsSystem.cleanup(simulationDatabase);
  mScriptingSystem.cleanup(simulationDatabase);
  mAudioSystem.cleanup(simulationDatabase);
}

void EditorSimulator::useSimulationUpdate() {
  mUpdater = [this](float dt, liquid::EntityDatabase &entityDatabase) {
    updateSimulation(dt, entityDatabase);
  };
}

void EditorSimulator::useEditorUpdate() {
  mUpdater = [this](float dt, liquid::EntityDatabase &entityDatabase) {
    updateEditor(dt, entityDatabase);
  };
}

void EditorSimulator::updateEditor(float dt,
                                   liquid::EntityDatabase &entityDatabase) {
  mCameraAspectRatioUpdater.update(entityDatabase);
  mEditorCamera.update();

  mSkeletonUpdater.update(entityDatabase);
  mSceneUpdater.update(entityDatabase);

  mEntityDeleter.update(entityDatabase);
}

void EditorSimulator::updateSimulation(float dt,
                                       liquid::EntityDatabase &entityDatabase) {
  mCameraAspectRatioUpdater.update(entityDatabase);

  mScriptingSystem.start(entityDatabase);
  mScriptingSystem.update(dt, entityDatabase);
  mAnimationSystem.update(dt, entityDatabase);

  mSkeletonUpdater.update(entityDatabase);
  mSceneUpdater.update(entityDatabase);

  mPhysicsSystem.update(dt, entityDatabase);
  mAudioSystem.output(entityDatabase);

  mEntityDeleter.update(entityDatabase);
}

} // namespace liquidator
