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

namespace liquidator {

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
  EditorSimulator(liquid::EventSystem &eventSystem, liquid::Window &window,
                  liquid::AssetRegistry &assetRegistry,
                  EditorCamera &editorCamera);

  /**
   * @brief Main update function
   *
   * Uses simulation and editing
   * updates
   *
   * @param dt Time delta
   * @param entityDatabase Entity database
   */
  void update(float dt, liquid::EntityDatabase &entityDatabase);

  /**
   * @brief Cleanup simulation database
   *
   * @param simulationDatabase Simulation database
   */
  void cleanupSimulationDatabase(liquid::EntityDatabase &simulationDatabase);

  /**
   * @brief Switch to simulation updater
   */
  void useSimulationUpdate();

  /**
   * @brief Switch to editing updater
   */
  void useEditorUpdate();

  /**
   * @brief Get physics system
   *
   * @return Physics system
   */
  inline liquid::PhysicsSystem &getPhysicsSystem() { return mPhysicsSystem; }

private:
  /**
   * @brief Editor updater
   *
   * @param dt Time delta
   * @param entityDatabase Entity database
   */
  void updateEditor(float dt, liquid::EntityDatabase &entityDatabase);

  /**
   * @brief Simulation updater
   *
   * @param dt Time delta
   * @param entityDatabase Entity database
   */
  void updateSimulation(float dt, liquid::EntityDatabase &entityDatabase);

private:
  std::function<void(float, liquid::EntityDatabase &)> mUpdater;

  EditorCamera &mEditorCamera;
  liquid::CameraAspectRatioUpdater mCameraAspectRatioUpdater;
  liquid::EntityDeleter mEntityDeleter;
  liquid::SkeletonUpdater mSkeletonUpdater;
  liquid::SceneUpdater mSceneUpdater;
  liquid::AnimationSystem mAnimationSystem;
  liquid::ScriptingSystem mScriptingSystem;
  liquid::PhysicsSystem mPhysicsSystem;
  liquid::AudioSystem<liquid::DefaultAudioBackend> mAudioSystem;
};

} // namespace liquidator
