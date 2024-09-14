#pragma once

#include "quoll/asset/AssetHandle.h"
#include "quoll/asset/Result.h"
#include "quoll/entity/Entity.h"
#include "quoll/scene/SceneAsset.h"
#include "quoll/yaml/Yaml.h"

namespace quoll {

class AssetCache;
struct Scene;

class SceneIO {
public:
  SceneIO(AssetCache &assetCache, Scene &scene);

  std::vector<Entity> loadScene(const AssetRef<SceneAsset> &scene);

  void reset();

private:
  Result<Entity> createEntityFromNode(const YAML::Node &node);

private:
  Scene &mScene;
  AssetCache &mAssetCache;

  std::unordered_map<u64, Entity> mEntityIdCache;
};

} // namespace quoll
