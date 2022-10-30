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

void EditorSimulator::update(float dt, liquid::Scene &scene) {
  mUpdater(dt, scene);
}

void EditorSimulator::cleanupSimulationDatabase(
    liquid::EntityDatabase &simulationDatabase) {
  mPhysicsSystem.cleanup(simulationDatabase);
  mScriptingSystem.cleanup(simulationDatabase);
  mAudioSystem.cleanup(simulationDatabase);
}

void EditorSimulator::useSimulationUpdate() {
  mUpdater = [this](float dt, liquid::Scene &scene) {
    updateSimulation(dt, scene);
  };
}

void EditorSimulator::useEditorUpdate() {
  mUpdater = [this](float dt, liquid::Scene &scene) {
    updateEditor(dt, scene);
  };
}

void EditorSimulator::updateEditor(float dt, liquid::Scene &scene) {
  auto &entityDatabase = scene.entityDatabase;
  mCameraAspectRatioUpdater.update(entityDatabase);
  mEditorCamera.update();

  mSkeletonUpdater.update(entityDatabase);
  mSceneUpdater.update(entityDatabase);

  mEntityDeleter.update(scene);
}

void EditorSimulator::updateSimulation(float dt, liquid::Scene &scene) {
  auto &entityDatabase = scene.entityDatabase;

  mCameraAspectRatioUpdater.update(entityDatabase);

  mScriptingSystem.start(entityDatabase);
  mScriptingSystem.update(dt, entityDatabase);
  mAnimationSystem.update(dt, entityDatabase);

  mSkeletonUpdater.update(entityDatabase);
  mSceneUpdater.update(entityDatabase);

  mPhysicsSystem.update(dt, entityDatabase);
  mAudioSystem.output(entityDatabase);

  mEntityDeleter.update(scene);
}

} // namespace liquidator
