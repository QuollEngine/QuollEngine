#pragma

#include "quoll/asset/AssetRegistry.h"
#include "quoll/asset/Result.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/yaml/Yaml.h"

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
