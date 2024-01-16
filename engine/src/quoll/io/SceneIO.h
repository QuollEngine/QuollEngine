#pragma once

#include "quoll/asset/AssetRegistry.h"
#include "quoll/asset/Result.h"
#include "quoll/yaml/Yaml.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/scene/Scene.h"

namespace quoll {

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
