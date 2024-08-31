#pragma once

#include "quoll/core/EntityDeleter.h"
#include "quoll/animation/AnimationSystem.h"
#include "quoll/asset/AssetCache.h"
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
                    AssetCache &assetCache);

  void prepare(SystemView &view);
  void cleanup(SystemView &view);
  void fixedUpdate(f32 dt, SystemView &view);
  void update(f32 dt, SystemView &view);
  void render(SystemView &view);

  SystemView createSystemView(Scene &scene);

  inline CameraAspectRatioUpdater &getCameraAspectRatioUpdater() {
    return mCameraAspectRatioUpdater;
  }

  inline UICanvasUpdater &getUICanvasUpdater() { return mUICanvasUpdater; }

  inline Window &getWindow() { return mWindow; }

private:
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
