#pragma once

#include "quoll/asset/AssetRegistry.h"
#include "quoll/asset/Result.h"
#include "quoll/yaml/Yaml.h"
#include "quoll/entity/EntityDatabase.h"

namespace quoll::detail {

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

  /**
   * @brief Load starting camera
   *
   * @param node YAML node
   * @param entityIdCache Entity ID cache
   * @return Found starting camera
   */
  Result<Entity> loadStartingCamera(const YAML::Node &node,
                                    EntityIdCache &entityIdCache);

  /**
   * @brief Load environment
   *
   * @param node YAML node
   * @param entityIdCache Entity ID cache
   * @return Found environment
   */
  Result<Entity> loadEnvironment(const YAML::Node &node,
                                 EntityIdCache &entityIdCache);

private:
  AssetRegistry &mAssetRegistry;
  EntityDatabase &mEntityDatabase;
};

} // namespace quoll::detail
