#pragma once

#include "EntityDatabase.h"
#include "liquid/asset/AssetRegistry.h"

namespace liquid {

/**
 * @brief Entity spawner
 */
class EntitySpawner {
public:
  /**
   * @brief Create entity spawner
   *
   * @param entityDatabase Entity database
   * @param assetRegistry Asset registry
   */
  EntitySpawner(EntityDatabase &entityDatabase, AssetRegistry &assetRegistry);

  /**
   * @brief Spawn empty entity
   *
   * @param transform Local transform
   * @return Entity
   */
  Entity spawnEmpty(LocalTransform transform);

  /**
   * @brief Spawn prefab
   *
   * @param handle Prefab handle
   * @param transform Local transform
   * @return All created entities
   */
  std::vector<Entity> spawnPrefab(PrefabAssetHandle handle,
                                  LocalTransform transform);

private:
  EntityDatabase &mEntityDatabase;
  AssetRegistry &mAssetRegistry;
};

} // namespace liquid
