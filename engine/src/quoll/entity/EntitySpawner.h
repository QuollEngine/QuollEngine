#pragma once

#include "EntityDatabase.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/scene/LocalTransform.h"

namespace quoll {

class EntitySpawner {
public:
  EntitySpawner(EntityDatabase &entityDatabase, AssetRegistry &assetRegistry);

  Entity spawnEmpty(LocalTransform transform);

  std::vector<Entity> spawnPrefab(PrefabAssetHandle handle,
                                  LocalTransform transform);

  Entity spawnSprite(TextureAssetHandle handle, LocalTransform transform);

private:
  EntityDatabase &mEntityDatabase;
  AssetRegistry &mAssetRegistry;
};

} // namespace quoll
