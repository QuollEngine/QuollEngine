#include "quoll/core/Base.h"
#include "quoll/audio/MiniAudioBackend.h"
#include "quoll/physx/PhysxBackend.h"
#include "quoll/scene/Scene.h"
#include "MainEngineModules.h"

namespace quoll {

MainEngineModules::MainEngineModules(InputDeviceManager &deviceManager,
                                     Window &window,
                                     AssetRegistry &assetRegistry)
    : mWindow(window), mInputMapSystem(deviceManager, assetRegistry),
      mScriptingSystem(assetRegistry), mAnimationSystem(assetRegistry),
      mPhysicsSystem(new PhysxBackend),
      mAudioSystem(new MiniAudioBackend, assetRegistry),
      mAssetRegistry(assetRegistry) {}

void MainEngineModules::prepare(SystemView &view) {
  mEntityDeleter.update(view);
  mCameraAspectRatioUpdater.update(view);
  mSkeletonUpdater.update(view);
  mSceneUpdater.update(view);
  mAnimationSystem.prepare(view);
}

void MainEngineModules::cleanup(SystemView &view) {
  mPhysicsSystem.cleanup(view);
  mScriptingSystem.cleanup(view);
  mAudioSystem.cleanup(view);
}

void MainEngineModules::fixedUpdate(f32 dt, SystemView &view) {
  mPhysicsSystem.update(dt, view);

  mInputMapSystem.update(view);
  mScriptingSystem.start(view, mPhysicsSystem, mWindow.getSignals());
  mScriptingSystem.update(dt, view);
  mAnimationSystem.update(dt, view);
}

void MainEngineModules::update(f32 dt, SystemView &view) {
  mAudioSystem.output(view);
}

void MainEngineModules::render(SystemView &view) {
  mUICanvasUpdater.render(view, mAssetRegistry);
}

SystemView MainEngineModules::createSystemView(Scene &scene) {
  SystemView view{&scene};

  mScriptingSystem.createSystemViewData(view);
  mAudioSystem.createSystemViewData(view);
  mPhysicsSystem.createSystemViewData(view);

  return std::move(view);
}

} // namespace quoll
