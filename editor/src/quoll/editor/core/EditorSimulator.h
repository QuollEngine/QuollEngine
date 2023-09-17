#pragma once

#include "quoll/asset/AssetRegistry.h"

#include "quoll/core/EntityDeleter.h"
#include "quoll/scene/SceneUpdater.h"
#include "quoll/scene/SkeletonUpdater.h"
#include "quoll/scene/CameraAspectRatioUpdater.h"
#include "quoll/animation/AnimationSystem.h"
#include "quoll/scripting/ScriptingSystem.h"
#include "quoll/physics/PhysicsSystem.h"
#include "quoll/audio/AudioSystem.h"
#include "quoll/window/Window.h"
#include "quoll/input/InputMapSystem.h"

#include "quoll/editor/editor-scene/EditorCamera.h"
#include "quoll/editor/state/WorkspaceState.h"

namespace quoll::editor {

/**
 * @brief Manager editor simulation
 *
 * Provides different updaters for
 * editor simulation
 */
class EditorSimulator {
public:
  /**
   * @brief Create editor simulation
   *
   * @param deviceManager Device manager
   * @param eventSystem Event system
   * @param window Window
   * @param assetRegistry Asset registry
   * @param editorCamera Editor camera
   */
  EditorSimulator(InputDeviceManager &deviceManager, EventSystem &eventSystem,
                  Window &window, AssetRegistry &assetRegistry,
                  EditorCamera &editorCamera);

  /**
   * @brief Main update function
   *
   * Uses simulation and editing
   * updates
   *
   * @param dt Time delta
   * @param state Workspace state
   */
  void update(float dt, WorkspaceState &state);

  /**
   * @brief Get physics system
   *
   * @return Physics system
   */
  inline PhysicsSystem &getPhysicsSystem() { return mPhysicsSystem; }

private:
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
  void updateEditor(float dt, WorkspaceState &scene);

  /**
   * @brief Simulation updater
   *
   * @param dt Time delta
   * @param state Workspace state
   */
  void updateSimulation(float dt, WorkspaceState &scene);

private:
  std::function<void(float, WorkspaceState &)> mUpdater;

  EditorCamera &mEditorCamera;
  CameraAspectRatioUpdater mCameraAspectRatioUpdater;
  EntityDeleter mEntityDeleter;
  SkeletonUpdater mSkeletonUpdater;
  SceneUpdater mSceneUpdater;
  AnimationSystem mAnimationSystem;
  ScriptingSystem mScriptingSystem;
  PhysicsSystem mPhysicsSystem;
  AudioSystem<DefaultAudioBackend> mAudioSystem;
  InputMapSystem mInputMapSystem;

  WorkspaceMode mMode = WorkspaceMode::Edit;
};

} // namespace quoll::editor
