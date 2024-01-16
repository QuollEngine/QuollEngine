#pragma once

#include "quoll/asset/AssetRegistry.h"
#include "quoll/core/EntityDeleter.h"
#include "quoll/scene/SceneUpdater.h"
#include "quoll/skeleton/SkeletonUpdater.h"
#include "quoll/scene/CameraAspectRatioUpdater.h"
#include "quoll/animation/AnimationSystem.h"
#include "quoll/lua-scripting/LuaScriptingSystem.h"
#include "quoll/physics/PhysicsSystem.h"
#include "quoll/audio/AudioSystem.h"
#include "quoll/window/Window.h"
#include "quoll/input/InputMapSystem.h"
#include "quoll/ui/UICanvasUpdater.h"

namespace quoll {

/**
 * @brief Main engine modules
 */
class MainEngineModules {
public:
  /**
   * @brief Create main engine modules
   *
   * @param deviceManager Device manager
   * @param window Window
   * @param assetRegistry Asset registry
   */
  MainEngineModules(InputDeviceManager &deviceManager, Window &window,
                    AssetRegistry &assetRegistry);

  /**
   * @brief Observe changes
   *
   * @param entityDatabase Entity database
   */
  void observeChanges(EntityDatabase &entityDatabase);

  /**
   * @brief Cleanup observers
   *
   * @param entityDatabase Entity database
   */
  void cleanupObservers(EntityDatabase &entityDatabase);

  /**
   * @brief Prepare data
   *
   * @param scene Scene
   */
  void prepare(Scene &scene);

  /**
   * @brief Fixed update
   *
   * @param dt Time delta
   * @param scene Scene
   */
  void fixedUpdate(f32 dt, Scene &scene);

  /**
   * @brief Update
   *
   * @param dt Time delta
   * @param scene Scene
   */
  void update(f32 dt, Scene &scene);

  /**
   * @brief Render
   *
   * @param scene Scene
   */
  void render(Scene &scene);

  /**
   * @brief Get camera aspect ratio updater
   *
   * @return Camera aspect ratio updater
   */
  inline CameraAspectRatioUpdater &getCameraAspectRatioUpdater() {
    return mCameraAspectRatioUpdater;
  }

  /**
   * @brief Get UI canvas updater
   *
   * @return UI canvas updater
   */
  inline UICanvasUpdater &getUICanvasUpdater() { return mUICanvasUpdater; }

  /**
   * @brief Get window
   *
   * @return Window
   */
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
