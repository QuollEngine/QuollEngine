#pragma once

#include "liquid/entity/EntityDatabase.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/renderer/Renderer.h"
#include "liquid/scene/SceneIO.h"

#include "EditorCamera.h"

namespace liquidator {

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
   * @param renderer Renderer
   * @param scenePath Scene path
   */
  EntityManager(liquid::AssetManager &assetManager, liquid::Renderer &renderer,
                const std::filesystem::path &scenePath);

  /**
   * @brief Save entity
   *
   * @param entity Entity
   */
  void save(liquid::Entity entity);

  /**
   * @brief Create empty entity
   *
   * @param parent Parent entity
   * @param transform Transform component
   * @param name Entity name
   * @return New entity
   */
  liquid::Entity
  createEmptyEntity(liquid::Entity parent,
                    const liquid::LocalTransformComponent &transform,
                    const liquid::String &name = "");

  /**
   * @brief Create empty entity at camera view
   *
   * @param camera Editor camera
   * @param parent Parent entity
   * @param name Entity name
   * @param saveToFile Save the created entities
   * @return New entity
   */
  liquid::Entity createEmptyEntity(EditorCamera &camera, liquid::Entity parent,
                                   const liquid::String &name = "New entity",
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
  void setSkeletonForEntity(liquid::Entity entity,
                            liquid::SkeletonAssetHandle handle);

  /**
   * @brief Toggle debug skeleton for entity
   *
   * @param entity Entity
   */
  void toggleSkeletonDebugForEntity(liquid::Entity entity);

  /**
   * @brief Set mesh for entiy
   *
   * @param entity Entity
   * @param handle Mesh asset handle
   */
  void setMesh(liquid::Entity entity, liquid::MeshAssetHandle handle);

  /**
   * @brief Set skinned mesh for entity
   *
   * @param entity Entity
   * @param handle Skinned mesh asset handle
   */
  void setSkinnedMesh(liquid::Entity entity,
                      liquid::SkinnedMeshAssetHandle handle);

  /**
   * @brief Set entity name
   *
   * @param entity Entity
   * @param name Name
   */
  void setName(liquid::Entity entity, const liquid::String &name);

  /**
   * @brief Set camera for entity
   *
   * @param entity Entity
   * @param lens Perspective lens component
   * @param autoRatio Calculate aspect ratio automatically
   */
  void setCamera(liquid::Entity entity,
                 const liquid::PerspectiveLensComponent &lens, bool autoRatio);

  /**
   * @brief Set audio for entity
   *
   * @param entity Entity
   * @param source Audio source asset
   */
  void setAudio(liquid::Entity entity, liquid::AudioAssetHandle source);

  /**
   * @brief Set text for entity
   *
   * @param entity Entity
   * @param text Text component
   */
  void setText(liquid::Entity entity, liquid::TextComponent text);

  /**
   * @brief Set script for entity
   *
   * @param entity Entity
   * @param handle Script handle
   */
  void setScript(liquid::Entity entity, liquid::LuaScriptAssetHandle handle);

  /**
   * @brief Delete entity
   *
   * @param entity Entity
   */
  void deleteEntity(liquid::Entity entity);

  /**
   * @brief Update local transform using world transform
   *
   * @param entity Entity to update
   * @param worldTransform World transform
   */
  void updateLocalTransformUsingWorld(liquid::Entity entity,
                                      const glm::mat4 &worldTransform);

  /**
   * @brief Spawn entity at view
   *
   * @param camera Editor camera
   * @param parent Parent entity
   * @param asset Asset handle
   * @param type Asset type
   * @param saveToFile Save the spawned entities
   * @return New entity
   */
  liquid::Entity spawnEntity(EditorCamera &camera, liquid::Entity parent,
                             uint32_t asset, liquid::AssetType type,
                             bool saveToFile = true);

  /**
   * @brief Get active entity database
   *
   * @return Active entity database
   */
  inline liquid::EntityDatabase &getActiveEntityDatabase() {
    return mInSimulation ? mSimulationEntityDatabase : mEntityDatabase;
  }

  /**
   * @brief Use simulation database
   */
  void useSimulationDatabase();

  /**
   * @brief Use editing database
   */
  void useEditingDatabase();

  /**
   * @brief Check if using simulation database
   *
   * @retval true Using simulation databse
   * @retval false Using editing database
   */
  inline bool isUsingSimulationDatabase() const { return mInSimulation; }

private:
  /**
   * @brief Get transform from camera
   *
   * @param camera Editor camera
   * @return Transform component
   */
  liquid::LocalTransformComponent
  getTransformFromCamera(EditorCamera &camera) const;

  /**
   * @brief Update simulation entity database
   *
   * Copies entity database to simulation
   * entity database
   */
  void updateSimulationEntityDatabase();

private:
  liquid::EntityDatabase mEntityDatabase;
  liquid::EntityDatabase mSimulationEntityDatabase;
  liquid::AssetManager &mAssetManager;
  liquid::SceneIO mSceneIO;
  bool mInSimulation = false;
  liquid::Renderer &mRenderer;
  std::filesystem::path mScenePath;
};

} // namespace liquidator
