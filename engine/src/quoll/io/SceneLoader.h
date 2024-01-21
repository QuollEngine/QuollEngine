#pragma once

#include "quoll/asset/Result.h"
#include "quoll/yaml/Yaml.h"

namespace quoll {

class EntityDatabase;
class AssetRegistry;

} // namespace quoll

namespace quoll::detail {

using EntityIdCache = std::unordered_map<u64, Entity>;

class SceneLoader {
public:
  SceneLoader(AssetRegistry &assetRegistry, EntityDatabase &entityDatabase);

  Result<bool> loadComponents(const YAML::Node &node, Entity entity,
                              EntityIdCache &entityIdCache);

  Result<Entity> loadStartingCamera(const YAML::Node &node,
                                    EntityIdCache &entityIdCache);

  Result<Entity> loadEnvironment(const YAML::Node &node,
                                 EntityIdCache &entityIdCache);

private:
  AssetRegistry &mAssetRegistry;
  EntityDatabase &mEntityDatabase;
};

} // namespace quoll::detail
