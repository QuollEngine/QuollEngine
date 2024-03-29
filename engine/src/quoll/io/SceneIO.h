#pragma once

#include "quoll/asset/Asset.h"
#include "quoll/asset/Result.h"
#include "quoll/entity/Entity.h"
#include "quoll/yaml/Yaml.h"

namespace quoll {

class AssetRegistry;
struct Scene;

class SceneIO {
public:
  SceneIO(AssetRegistry &assetRegistry, Scene &scene);

  std::vector<Entity> loadScene(SceneAssetHandle scene);

  void reset();

private:
  Result<Entity> createEntityFromNode(const YAML::Node &node);

private:
  Scene &mScene;
  AssetRegistry &mAssetRegistry;

  std::unordered_map<u64, Entity> mEntityIdCache;
};

} // namespace quoll
