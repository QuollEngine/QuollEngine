#pragma once

#include "liquid/yaml/Yaml.h"

namespace liquid {

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

} // namespace liquid
