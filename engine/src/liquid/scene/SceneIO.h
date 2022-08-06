#pragma once

#include "liquid/asset/AssetRegistry.h"
#include "liquid/asset/Result.h"
#include "liquid/yaml/Yaml.h"
#include "liquid/entity/EntityDatabase.h"

#include "private/SceneLoader.h"
#include "private/EntitySerializer.h"

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
   * @param entityDatabase Entity database
   */
  SceneIO(AssetRegistry &assetRegistry, EntityDatabase &entityDatabase);

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

private:
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
  EntityDatabase &mEntityDatabase;
  detail::SceneLoader mLoader;
  detail::EntitySerializer mDeserializer;

  std::unordered_map<uint64_t, Entity> mEntityIdCache;
  uint64_t mLastId = 1;
};

} // namespace liquid
