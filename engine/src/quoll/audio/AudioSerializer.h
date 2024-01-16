#pragma once

#include "quoll/io/SerializerBase.h"

namespace quoll {

class AudioSerializer {
public:
  static void serialize(YAML::Node &node, EntityDatabase &entityDatabase,
                        Entity entity, AssetRegistry &assetRegistry);

  static void deserialize(const YAML::Node &node,
                          EntityDatabase &entityDatabase, Entity entity,
                          AssetRegistry &assetRegistry);
};

} // namespace quoll