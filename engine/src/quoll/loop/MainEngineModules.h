#pragma once

#include "quoll/core/EntityDeleter.h"
#include "quoll/animation/AnimationSystem.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/audio/AudioSystem.h"
#include "quoll/input/InputMapSystem.h"
#include "quoll/lua-scripting/LuaScriptingSystem.h"
#include "quoll/physics/PhysicsSystem.h"
#include "quoll/scene/CameraAspectRatioUpdater.h"
#include "quoll/scene/SceneUpdater.h"
#include "quoll/skeleton/SkeletonUpdater.h"
#include "quoll/ui/UICanvasUpdater.h"
#include "quoll/window/Window.h"

namespace quoll {

/**
 * Main engine modules
 *
 * Maintains all the main engine modules
 * required to run a simulation and provides
 * the execution order of system updates.
 */
class MainEngineModules {
public:
  MainEngineModules(InputDeviceManager &deviceManager, Window &window,
                    AssetRegistry &assetRegistry);

  void observeChanges(EntityDatabase &entityDatabase);

  void cleanupObservers(EntityDatabase &entityDatabase);

  void prepare(Scene &scene);

  void fixedUpdate(f32 dt, Scene &scene);

  void update(f32 dt, Scene &scene);

  void render(Scene &scene);

  inline CameraAspectRatioUpdater &getCameraAspectRatioUpdater() {
    return mCameraAspectRatioUpdater;
  }

  inline UICanvasUpdater &getUICanvasUpdater() { return mUICanvasUpdater; }

  inline Window &getWindow() { return mWindow; }

private:
  AssetRegistry &mAssetRegistry;
  Window &mWindow;

  CameraAspectRatioUpdater mCameraAspectRatioUpdater;
  EntityDeleter mEntityDeleter;
  SkeletonUpdater mSkeletonUpdater;
  SceneUpdater mSceneUpdater;
  AnimationSystem mAnimationSystem;
  LuaScriptingSystem mScriptingSystem;
  PhysicsSystem mPhysicsSystem;
  AudioSystem<DefaultAudioBackend> mAudioSystem;
  InputMapSystem mInputMapSystem;
  UICanvasUpdater mUICanvasUpdater;
};

} // namespace quoll
