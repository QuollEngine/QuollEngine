#pragma once

#include "quoll/scene/Scene.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/editor/asset/AssetSyncer.h"

#include "quoll/yaml/Yaml.h"

namespace quoll::editor {

/**
 * @brief Scene writer
 *
 * Writes scene to a file
 */
class SceneWriter : public AssetSyncer {
public:
  /**
   * @brief Create scene writer
   *
   * @param scene Scene
   * @param assetRegistry Asset registry
   */
  SceneWriter(Scene &scene, AssetRegistry &assetRegistry);

  SceneWriter(const SceneWriter &) = delete;
  SceneWriter &operator=(const SceneWriter &) = delete;
  SceneWriter(SceneWriter &&) = delete;
  SceneWriter &operator=(SceneWriter &&) = delete;

  /**
   * @brief Virtual destructor
   */
  virtual ~SceneWriter() = default;

  /**
   * @brief Open scene file
   *
   * @param sourcePath
   */
  void open(Path sourcePath);

  /**
   * @brief Save entities
   *
   * @param entities Entities
   */
  void syncEntities(const std::vector<Entity> &entities) override;

  /**
   * @brief Delete entities
   *
   * @param entities Entities
   */
  void deleteEntities(const std::vector<Entity> &entities) override;

  /**
   * @brief Save scene
   *
   * Saves starting camera and environment
   */
  void syncScene() override;

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
  u64 generateId();

private:
  Scene &mScene;
  Path mSourcePath;
  AssetRegistry &mAssetRegistry;

  std::fstream mStream;
  YAML::Node mRoot;

  std::unordered_map<u64, Entity> mEntityIdCache;

  u64 mLastId = 1;
};

} // namespace quoll::editor
