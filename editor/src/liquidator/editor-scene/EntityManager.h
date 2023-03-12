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
   * @brief Create empty entity at camera view
   *
   * @param camera Camera
   * @param parent Parent entity
   * @param name Entity name
   * @param saveToFile Save the created entities
   * @return New entity
   */
  Entity createEmptyEntity(Entity camera, Entity parent,
                           const String &name = "New entity",
                           bool saveToFile = true);

  /**
   * @brief Load scene
   *
   * @retval true Scene loaded
   * @retval false Scene did not load
   */
  bool loadScene();

  /**
   * @brief Set skeleton for entity
   *
   * @param entity Entity
   * @param handle Skeleton asset handle
   */
  void setSkeletonForEntity(Entity entity, SkeletonAssetHandle handle);

  /**
   * @brief Set mesh for entiy
   *
   * @param entity Entity
   * @param handle Mesh asset handle
   */
  void setMesh(Entity entity, MeshAssetHandle handle);

  /**
   * @brief Set skinned mesh for entity
   *
   * @param entity Entity
   * @param handle Skinned mesh asset handle
   */
  void setSkinnedMesh(Entity entity, SkinnedMeshAssetHandle handle);

  /**
   * @brief Set entity name
   *
   * @param entity Entity
   * @param name Name
   */
  void setName(Entity entity, const String &name);

  /**
   * @brief Update local transform using world transform
   *
   * @param entity Entity to update
   * @param worldTransform World transform
   */
  void updateLocalTransformUsingWorld(Entity entity,
                                      const glm::mat4 &worldTransform);

  /**
   * @brief Spawn entity at camera view
   *
   * @param camera Camera
   * @param parent Parent entity
   * @param asset Asset handle
   * @param type Asset type
   * @param saveToFile Save the spawned entities
   * @return New entity
   */
  Entity spawnEntity(Entity camera, Entity parent, uint32_t asset,
                     AssetType type, bool saveToFile = true);

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
   * @brief Check if using simulation database
   *
   * @retval true Using simulation databse
   * @retval false Using editing database
   */
  inline bool isUsingSimulationDatabase() const {
    return mState.mode == WorkspaceMode::Simulation;
  }

  /**
   * @brief Get active camera in simulation
   *
   * @return Active simulation camera
   */
  Entity getActiveSimulationCamera();

  /**
   * @brief Get active scene
   *
   * @return Active scene
   */
  inline Scene &getActiveScene() {
    return mState.mode == WorkspaceMode::Simulation ? mState.simulationScene
                                                    : mState.scene;
  }

  /**
   * @brief Save environment
   */
  void saveEnvironment();

private:
  /**
   * @brief Get transform from camera
   *
   * @param camera Camera
   * @return Transform component
   */
  LocalTransform getTransformFromCamera(Entity camera) const;

private:
  WorkspaceState &mState;

  AssetManager &mAssetManager;
  SceneIO &mSceneIO;
  std::filesystem::path mScenePath;
};

} // namespace liquid::editor
