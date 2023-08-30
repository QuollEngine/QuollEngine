#pragma once

#include "EntityDatabase.h"
#include "quoll/asset/AssetRegistry.h"

namespace quoll {

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
   * @return Spawned entity
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

  /**
   * Spawn sprite
   *
   * @param handle Texture asset handle
   * @param transform Local transform
   * @return Spawned entity
   */
  Entity spawnSprite(TextureAssetHandle handle, LocalTransform transform);

private:
  EntityDatabase &mEntityDatabase;
  AssetRegistry &mAssetRegistry;
};

} // namespace quoll
