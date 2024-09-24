#pragma

#include "quoll/core/Result.h"
#include "quoll/entity/Entity.h"
#include "quoll/yaml/Yaml.h"

namespace quoll {

class AssetRegistry;
class EntityDatabase;

} // namespace quoll

namespace quoll::detail {

class EntitySerializer {
public:
  EntitySerializer(AssetRegistry &assetRegistry,
                   EntityDatabase &entityDatabase);

  Result<YAML::Node> serialize(Entity entity);

  YAML::Node createComponentsNode(Entity entity);

private:
  AssetRegistry &mAssetRegistry;
  EntityDatabase &mEntityDatabase;
};

} // namespace quoll::detail
