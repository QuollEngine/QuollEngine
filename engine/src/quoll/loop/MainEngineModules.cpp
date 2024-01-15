#include "quoll/core/Base.h"
#include "MainEngineModules.h"

namespace quoll {

MainEngineModules::MainEngineModules(InputDeviceManager &deviceManager,
                                     Window &window,
                                     AssetRegistry &assetRegistry)
    : mWindow(window), mInputMapSystem(deviceManager, assetRegistry),
      mScriptingSystem(assetRegistry), mAnimationSystem(assetRegistry),
      mPhysicsSystem(PhysicsSystem::createPhysxBackend()),
      mAudioSystem(assetRegistry), mAssetRegistry(assetRegistry) {}

void MainEngineModules::observeChanges(EntityDatabase &entityDatabase) {
  mPhysicsSystem.observeChanges(entityDatabase);
  mScriptingSystem.observeChanges(entityDatabase);
  mAudioSystem.observeChanges(entityDatabase);
}

void MainEngineModules::cleanupObservers(EntityDatabase &entityDatabase) {
  mPhysicsSystem.cleanup(entityDatabase);
  mScriptingSystem.cleanup(entityDatabase);
  mAudioSystem.cleanup(entityDatabase);
}

void MainEngineModules::prepare(Scene &scene) {
  mEntityDeleter.update(scene);
  mCameraAspectRatioUpdater.update(scene.entityDatabase);
  mSkeletonUpdater.update(scene.entityDatabase);
  mSceneUpdater.update(scene.entityDatabase);
  mAnimationSystem.prepare(scene.entityDatabase);
}

void MainEngineModules::fixedUpdate(f32 dt, Scene &scene) {
  mPhysicsSystem.update(dt, scene.entityDatabase);

  mInputMapSystem.update(scene.entityDatabase);
  mScriptingSystem.start(scene.entityDatabase, mPhysicsSystem,
                         mWindow.getSignals());
  mScriptingSystem.update(dt, scene.entityDatabase);
  mAnimationSystem.update(dt, scene.entityDatabase);
}

void MainEngineModules::update(f32 dt, Scene &scene) {
  mAudioSystem.output(scene.entityDatabase);
}

void MainEngineModules::render(Scene &scene) {
  mUICanvasUpdater.render(scene.entityDatabase, mAssetRegistry);
}

} // namespace quoll
