#pragma once

#include "quoll/yaml/Yaml.h"

namespace quoll {

/**
 * @brief Scene asset
 */
struct SceneAsset {

  /**
   * Yaml node that stores
   * the whole scene
   */
  YAML::Node data;
};

} // namespace quoll
