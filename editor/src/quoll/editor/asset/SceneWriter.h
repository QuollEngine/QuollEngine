#pragma once

#include "quoll/scene/Scene.h"
#include "quoll/asset/AssetRegistry.h"

#include "quoll/yaml/Yaml.h"

namespace quoll::editor {

/**
 * @brief Scene writer
 *
 * Writes scene to a file
 */
class SceneWriter {
public:
  /**
   * @brief Create scene writer
   *
   * @param scene Scene
   * @param assetRegistry Asset registry
   */
  SceneWriter(Scene &scene, AssetRegistry &assetRegistry);

  /**
   * @brief Open scene file
   *
   * @param sourcePath
   */
  void open(Path sourcePath);

  /**
   * @brief Save entities
   *
   * @param entities
   */
  void saveEntities(const std::vector<Entity> &entities);

  /**
   * @brief Delete entities
   *
   * @param entities
   */
  void deleteEntities(const std::vector<Entity> &entities);

  /**
   * @brief Save scene
   *
   * Saves starting camera and environment
   */
  void saveScene();

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

  /**
   * @brief Update starting camera in scene YAML
   */
  void updateStartingCamera();

  /**
   * @brief Update environment in scene YAML
   */
  void updateEnvironment();

  /**
   * @brief Save scene file
   */
  void save();

  /**
   * @brief Generate unique ID for entity
   *
   * @return Generated ID
   */
  uint64_t generateId();

private:
  Scene &mScene;
  Path mSourcePath;
  AssetRegistry &mAssetRegistry;

  std::fstream mStream;
  YAML::Node mRoot;

  std::unordered_map<uint64_t, Entity> mEntityIdCache;

  uint64_t mLastId = 1;
};

} // namespace quoll::editor
