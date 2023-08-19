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
   * @brief Save entities
   *
   * @param entities Entities
   * @param path Scene path
   */
  void saveEntities(const std::vector<Entity> &entities, const Path &path);

  /**
   * @brief Delete entities and all descendants
   *
   * @param entities Entities
   * @param path Scene path
   */
  void deleteEntities(const std::vector<Entity> &entities, const Path &path);

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
   * @brief Reset everything
   *
   * Clear cache, destroy the entity database,
   * and create dummy camera component
   */
  void reset();

private:
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
  /**
   * @brief Update entity in scene YAML
   *
   * @param entity Entity
   * @param node YAML node
   * @param updateCache Entity update cache
   */
  void updateSceneYaml(Entity entity, YAML::Node &node,
                       std::unordered_map<Entity, bool> &updateCache);

  /**
   * @brief Remove entity in scene YAML
   *
   * @param entity Entity
   * @param node YAML node
   * @param deleteCache Entity delete cache
   */
  void removeEntityFromSceneYaml(Entity entity, YAML::Node &node,
                                 std::unordered_map<Entity, bool> &deleteCache);

private:
  Scene &mScene;
  AssetRegistry &mAssetRegistry;

  std::unordered_map<uint64_t, Entity> mEntityIdCache;

  uint64_t mLastId = 1;
};

} // namespace liquid
