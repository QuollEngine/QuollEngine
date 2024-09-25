#pragma once

#include "quoll/core/Result.h"
#include "quoll/yaml/Yaml.h"

namespace quoll {

class EntityDatabase;
class AssetCache;

} // namespace quoll

namespace quoll::detail {

using EntityIdCache = std::unordered_map<u64, Entity>;

class SceneLoader {
public:
  SceneLoader(AssetCache &assetCache, EntityDatabase &entityDatabase);

  Result<void> loadComponents(const YAML::Node &node, Entity entity,
                              EntityIdCache &entityIdCache);

  Result<Entity> loadStartingCamera(const YAML::Node &node,
                                    EntityIdCache &entityIdCache);

  Result<Entity> loadEnvironment(const YAML::Node &node,
                                 EntityIdCache &entityIdCache);

private:
  AssetCache &mAssetCache;
  EntityDatabase &mEntityDatabase;
};

} // namespace quoll::detail
