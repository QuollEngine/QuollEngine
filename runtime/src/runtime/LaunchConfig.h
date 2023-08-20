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
   * Starting scene UUID
   */
  String startingScene;
};

} // namespace liquid::runtime
