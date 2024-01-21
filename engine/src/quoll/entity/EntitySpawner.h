#pragma once

#include "quoll/asset/Asset.h"
#include "quoll/scene/LocalTransform.h"
#include "Entity.h"

namespace quoll {

class AssetRegistry;
class EntityDatabase;

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
