#pragma once

namespace liquid::runtime {

/**
 * @brief Launch configuration structure
 */
struct LaunchConfig {
  /**
   * Game name
   */
  String name;

  /**
   * Game assets path
   */
  Path assetsPath;

  /**
   * Game scenes path
   */
  Path scenesPath;
};

} // namespace liquid::runtime
