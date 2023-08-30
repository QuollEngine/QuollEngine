#pragma once

#include "liquid/asset/AssetRegistry.h"

#include "liquid/core/EntityDeleter.h"
#include "liquid/scene/SceneUpdater.h"
#include "liquid/scene/SkeletonUpdater.h"
#include "liquid/scene/CameraAspectRatioUpdater.h"
#include "liquid/animation/AnimationSystem.h"
#include "liquid/scripting/ScriptingSystem.h"
#include "liquid/physics/PhysicsSystem.h"
#include "liquid/audio/AudioSystem.h"
#include "liquid/window/Window.h"

#include "liquidator/editor-scene/EditorCamera.h"
#include "liquidator/state/WorkspaceState.h"

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
   * @param eventSystem Event system
   * @param window Window
   * @param assetRegistry Asset registry
   * @param editorCamera Editor camera
   */
  EditorSimulator(EventSystem &eventSystem, Window &window,
                  AssetRegistry &assetRegistry, EditorCamera &editorCamera);

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

  WorkspaceMode mMode = WorkspaceMode::Edit;
};

} // namespace quoll::editor
