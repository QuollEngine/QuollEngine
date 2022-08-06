#pragma once

#include "liquid/asset/AssetRegistry.h"
#include "liquid/asset/Result.h"
#include "liquid/yaml/Yaml.h"
#include "liquid/entity/EntityDatabase.h"

namespace liquid::detail {

using EntityIdCache = std::unordered_map<uint64_t, Entity>;

/**
 * @brief Load scene data to entity
 */
class SceneLoader {
public:
  /**
   * @brief Create scene loader
   *
   * @param assetRegistry Asset registry
   * @param entityDatabase Entity database
   */
  SceneLoader(AssetRegistry &assetRegistry, EntityDatabase &entityDatabase);

  /**
   * @brief Load entity components from YAML
   *
   * @param node YAML node
   * @param entity Entity
   * @param entityIdCache Entity ID cache
   * @return Load result
   */
  Result<bool> loadComponents(const YAML::Node &node, Entity entity,
                              EntityIdCache &entityIdCache);

private:
  AssetRegistry &mAssetRegistry;
  EntityDatabase &mEntityDatabase;
};

} // namespace liquid::detail
