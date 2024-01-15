#pragma once

#include "quoll/asset/AssetRegistry.h"
#include "quoll/core/EntityDeleter.h"
#include "quoll/scene/SceneUpdater.h"
#include "quoll/scene/SkeletonUpdater.h"
#include "quoll/scene/CameraAspectRatioUpdater.h"
#include "quoll/animation/AnimationSystem.h"
#include "quoll/lua-scripting/LuaScriptingSystem.h"
#include "quoll/physics/PhysicsSystem.h"
#include "quoll/audio/AudioSystem.h"
#include "quoll/window/Window.h"
#include "quoll/input/InputMapSystem.h"
#include "quoll/ui/UICanvasUpdater.h"

#include "quoll/editor/workspace/WorkspaceState.h"

#include "EditorCamera.h"

namespace quoll::editor {

/**
 * @brief Manager editor simulation
 *
 * Provides different updaters for
 * editor simulation
 */
class SceneSimulator {
public:
  /**
   * @brief Create editor simulation
   *
   * @param deviceManager Device manager
   * @param window Window
   * @param assetRegistry Asset registry
   * @param editorCamera Editor camera
   */
  SceneSimulator(InputDeviceManager &deviceManager, Window &window,
                 AssetRegistry &assetRegistry, EditorCamera &editorCamera);

  /**
   * @brief Render
   *
   * @param db Entity database
   */
  void render(EntityDatabase &db);

  /**
   * @brief Get physics system
   *
   * @return Physics system
   */
  inline PhysicsSystem &getPhysicsSystem() { return mPhysicsSystem; }

  /**
   * @brief Get camera aspect ratio updater
   *
   * @return Camera aspect ratio updater
   */
  inline CameraAspectRatioUpdater &getCameraAspectRatioUpdater() {
    return mCameraAspectRatioUpdater;
  }

  /**
   * @brief Get UI Canvas updater
   *
   * @return UI Canvas updater
   */
  inline UICanvasUpdater &getUICanvasUpdater() { return mUICanvasUpdater; }

  /**
   * @brief Get editor camera
   *
   * @return Editor camera
   */
  inline EditorCamera &getEditorCamera() { return mEditorCamera; }

  /**
   * @brief Get window
   *
   * @return Window
   */
  inline Window &getWindow() { return mWindow; }

public:
  /**
   * @brief Cleanup simulation database
   *
   * @param simulationDatabase Simulation database
   */
  void cleanupSimulationDatabase(EntityDatabase &simulationDatabase);

  /**
   * @brief Observe changes for simulation database
   *
   * @param simulationDatabase Simulation database
   */
  void observeChanges(EntityDatabase &simulationDatabase);

  /**
   * @brief Editor updater
   *
   * @param dt Time delta
   * @param state Workspace state
   */
  void updateEditor(f32 dt, WorkspaceState &state);

  /**
   * @brief Simulation updater
   *
   * @param dt Time delta
   * @param state Workspace state
   */
  void updateSimulation(f32 dt, WorkspaceState &state);

  /**
   * @brief Prepare data
   *
   * @param state Workspace state
   */
  void prepare(WorkspaceState &state);

private:
  AssetRegistry &mAssetRegistry;

  EditorCamera &mEditorCamera;
  CameraAspectRatioUpdater mCameraAspectRatioUpdater;
  EntityDeleter mEntityDeleter;
  SkeletonUpdater mSkeletonUpdater;
  SceneUpdater mSceneUpdater;
  AnimationSystem mAnimationSystem;
  LuaScriptingSystem mScriptingSystem;
  PhysicsSystem mPhysicsSystem;
  AudioSystem<DefaultAudioBackend> mAudioSystem;
  InputMapSystem mInputMapSystem;
  Window &mWindow;
  UICanvasUpdater mUICanvasUpdater;
};

} // namespace quoll::editor
