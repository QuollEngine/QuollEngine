#pragma once

#include "quoll/renderer/TextureAsset.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/PrefabAsset.h"
#include "Entity.h"

namespace quoll {

class AssetCache;
class EntityDatabase;

class EntitySpawner {
public:
  EntitySpawner(EntityDatabase &entityDatabase, AssetCache &assetCache);

  Entity spawnEmpty(LocalTransform transform);

  std::vector<Entity> spawnPrefab(AssetRef<PrefabAsset> prefab,
                                  LocalTransform transform);

  Entity spawnSprite(AssetRef<TextureAsset> texture, LocalTransform transform);

private:
  EntityDatabase &mEntityDatabase;
  AssetCache &mAssetCache;
};

} // namespace quoll
