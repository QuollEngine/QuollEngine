#pragma once

#include "liquid/asset/AssetRegistry.h"
#include "liquid/asset/Result.h"
#include "liquid/yaml/Yaml.h"
#include "liquid/entity/EntityDatabase.h"
#include "liquid/scene/Scene.h"

namespace liquid {

/**
 * @brief Scene writer and reader
 */
class SceneIO {
public:
  /**
   * @brief Create scene IO
   *
   * @param assetRegistry Asset registry
   * @param scene Scene
   */
  SceneIO(AssetRegistry &assetRegistry, Scene &scene);

  /**
   * @brief Load scene from a path
   *
   * @param path Scene path
   * @return List of entities
   */
  std::vector<Entity> loadScene(const Path &path);

  /**
   * @brief Save entity
   *
   * @param entity Entity
   * @param path Scene path
   */
  void saveEntity(Entity entity, const Path &path);

  /**
   * @brief Save starting camera
   *
   * @param path Scene path
   * @return Save result
   */
  Result<bool> saveStartingCamera(const Path &path);

  /**
   * @brief Save environment
   *
   * @param path Scene path
   * @return Save result
   */
  Result<bool> saveEnvironment(const Path &path);

  /**
   * @brief Delete entity
   *
   * @param entity Entity
   * @param path Scene path
   */
  void deleteEntityFilesAndRelations(Entity entity, const Path &path);

  /**
   * @brief Reset everything
   *
   * Clear cache, destroy the entity database,
   * and create dummy camera component
   */
  void reset();

private:
  /**
   * @brief Get entity path
   *
   * @param entity Entity
   * @param path Scene path
   * @return Entity path
   */
  Path getEntityPath(Entity entity, const Path &path);

  /**
   * @brief Load environment
   *
   * @param zone Zone
   */
  void loadEnvironment(const YAML::Node &zone);

  /**
   * @brief Generate unique ID for entity
   *
   * @return Generated ID
   */
  uint64_t generateId();

  /**
   * @brief Create entity from YAML node
   *
   * @param node YAML node
   * @return Entity or error
   */
  Result<Entity> createEntityFromNode(const YAML::Node &node);

private:
  Scene &mScene;
  AssetRegistry &mAssetRegistry;

  std::unordered_map<uint64_t, Entity> mEntityIdCache;
  uint64_t mLastId = 1;
};

} // namespace liquid
