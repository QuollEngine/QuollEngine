#pragma once

#include "quoll/io/SerializerBase.h"

namespace quoll {

class CameraSerializer {
public:
  static void serialize(YAML::Node &node, EntityDatabase &entityDatabase,
                        Entity entity);

  static void deserialize(const YAML::Node &node,
                          EntityDatabase &entityDatabase, Entity entity);
};

} // namespace quoll
