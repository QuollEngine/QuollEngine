#pragma

#include "liquid/asset/AssetRegistry.h"
#include "liquid/asset/Result.h"
#include "liquid/yaml/Yaml.h"
#include "liquid/entity/EntityDatabase.h"

namespace liquid::detail {

/**
 * @brief Serialize entity for writing
 */
class EntitySerializer {
public:
  /**
   * @brief Create serializer
   *
   * @param assetRegistry Asset registry
   * @param entityDatabase Entity database
   */
  EntitySerializer(AssetRegistry &assetRegistry,
                   EntityDatabase &entityDatabase);

  /**
   * @brief Serialize entity
   *
   * @param entity Entity
   * @return YAML node on success, error on failure
   */
  Result<YAML::Node> serialize(Entity entity);

  /**
   * @brief Create YAML node for entity components
   *
   * @param entity Entity
   * @return YAML node for entity components
   */
  YAML::Node createComponentsNode(Entity entity);

private:
  AssetRegistry &mAssetRegistry;
  EntityDatabase &mEntityDatabase;
};

} // namespace liquid::detail
