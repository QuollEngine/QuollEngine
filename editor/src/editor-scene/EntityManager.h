#pragma once

#include "liquid/entity/EntityContext.h"
#include "liquid/scene/Scene.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/renderer/Renderer.h"
#include "EditorCamera.h"

namespace liquidator {

class EntityManager {
public:
  /**
   * @brief Create entity manager
   *
   * @param entityContext Entity context
   * @param assetManager Asset manager
   * @param renderer Renderer
   * @param scenePath Scene path
   */
  EntityManager(liquid::EntityContext &entityContext,
                liquid::AssetManager &assetManager, liquid::Renderer &renderer,
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
   * @param parent Parent node
   * @param transform Transform component
   * @param name Entity name
   * @return New entity node
   */
  liquid::SceneNode *
  createEmptyEntity(liquid::SceneNode *parent,
                    const liquid::TransformComponent &transform,
                    const liquid::String &name = "");

  /**
   * @brief Create empty entity at camera view
   *
   * @param camera Editor camera
   * @param parent Parent node
   * @param name Entity name
   * @param saveToFile Save the created entities
   * @return New entity node
   */
  liquid::SceneNode *createEmptyEntity(EditorCamera &camera,
                                       liquid::SceneNode *parent,
                                       const liquid::String &name = "",
                                       bool saveToFile = true);

  /**
   * @brief Load scene
   *
   * @param parent Parent node to render to
   * @retval true Scene loaded
   * @retval false Scene did not load
   */
  bool loadScene(liquid::SceneNode *parent);

  /**
   * @brief Set skeleton for entity
   *
   * @param entity Entity
   * @param handle Skeleton asset handle
   */
  void setSkeletonForEntity(liquid::Entity entity,
                            liquid::SkeletonAssetHandle handle);

  /**
   * @brief Set mesh for entiy
   *
   * @param entity Entity
   * @param handle Mesh asset handle
   */
  void setMeshForEntity(liquid::Entity entity, liquid::MeshAssetHandle handle);

  /**
   * @brief Set skinned mesh for entity
   *
   * @param entity Entity
   * @param handle Skinned mesh asset handle
   */
  void setSkinnedMeshForEntity(liquid::Entity entity,
                               liquid::SkinnedMeshAssetHandle handle);

  /**
   * @brief Set entity name
   *
   * @param entity Entity
   * @param name Name
   */
  void setName(liquid::Entity entity, const liquid::String &name);

  /**
   * @brief Set camera
   *
   * @param entity Entity
   * @param lens Perspective lens component
   * @param autoRatio Calculate aspect ratio automatically
   */
  void createCamera(liquid::Entity entity,
                    const liquid::PerspectiveLensComponent &lens,
                    bool autoRatio);

  /**
   * @brief Delete entity
   *
   * @param entity Entity
   */
  void deleteEntity(liquid::Entity entity);

  /**
   * @brief Spawn entity at view
   *
   * @param camera Editor camera
   * @param parent Parent node
   * @param asset Asset handle
   * @param type Asset type
   * @param saveToFile Save the spawned entities
   * @return New entity
   */
  liquid::Entity spawnAsset(EditorCamera &camera, liquid::SceneNode *parent,
                            uint32_t asset, liquid::AssetType type,
                            bool saveToFile = true);

private:
  /**
   * @brief Get transform from camera
   *
   * @param camera Editor camera
   * @return Transform component
   */
  liquid::TransformComponent getTransformFromCamera(EditorCamera &camera) const;

private:
  liquid::EntityContext &mEntityContext;
  liquid::AssetManager &mAssetManager;
  liquid::Renderer &mRenderer;
  uint64_t mLastId = 1;
  std::filesystem::path mScenePath;
};

} // namespace liquidator