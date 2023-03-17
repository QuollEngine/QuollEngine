#pragma once

#include "liquid/scene/Scene.h"
#include "liquid/entity/EntityDatabase.h"
#include "liquid/renderer/Renderer.h"
#include "liquid/scene/SceneIO.h"

#include "liquidator/state/WorkspaceState.h"
#include "liquidator/asset/AssetManager.h"
#include "EditorCamera.h"

namespace liquid::editor {

/**
 * @brief Entity manager
 *
 * Creates, saves, and loads
 * entities
 */
class EntityManager {
public:
  /**
   * @brief Create entity manager
   *
   * @param assetManager Asset manager
   * @param sceneIO Scene IO
   * @param state Workspace state
   * @param scenePath Scene path
   */
  EntityManager(AssetManager &assetManager, SceneIO &sceneIO,
                WorkspaceState &state, const std::filesystem::path &scenePath);

  /**
   * @brief Save entity
   *
   * @param entity Entity
   */
  void save(Entity entity);

  /**
   * @brief Create empty entity
   *
   * @param parent Parent entity
   * @param transform Transform component
   * @param name Entity name
   * @return New entity
   */
  Entity createEmptyEntity(Entity parent, const LocalTransform &transform,
                           const String &name = "");

  /**
   * @brief Load scene
   *
   * @retval true Scene loaded
   * @retval false Scene did not load
   */
  bool loadScene();

  /**
   * @brief Get active entity database
   *
   * @return Active entity database
   */
  inline EntityDatabase &getActiveEntityDatabase() {
    return mState.mode == WorkspaceMode::Simulation
               ? mState.simulationScene.entityDatabase
               : mState.scene.entityDatabase;
  }

  /**
   * @brief Update local transform using world transform
   *
   * @param entity Entity to update
   * @param worldTransform World transform
   */
  void updateLocalTransformUsingWorld(Entity entity,
                                      const glm::mat4 &worldTransform);

private:
  /**
   * @brief Set entity name
   *
   * @param entity Entity
   * @param name Name
   */
  void setName(Entity entity, const String &name);

private:
  WorkspaceState &mState;

  AssetManager &mAssetManager;
  SceneIO &mSceneIO;
  std::filesystem::path mScenePath;
};

} // namespace liquid::editor
