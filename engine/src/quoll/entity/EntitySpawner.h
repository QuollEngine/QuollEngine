#pragma once

#include "quoll/asset/AssetHandle.h"
#include "quoll/renderer/TextureAsset.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/PrefabAsset.h"
#include "Entity.h"

namespace quoll {

class AssetRegistry;
class EntityDatabase;

class EntitySpawner {
public:
  EntitySpawner(EntityDatabase &entityDatabase, AssetRegistry &assetRegistry);

  Entity spawnEmpty(LocalTransform transform);

  std::vector<Entity> spawnPrefab(AssetHandle<PrefabAsset> handle,
                                  LocalTransform transform);

  Entity spawnSprite(AssetHandle<TextureAsset> handle,
                     LocalTransform transform);

private:
  EntityDatabase &mEntityDatabase;
  AssetRegistry &mAssetRegistry;
};

} // namespace quoll
