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
   * Starting scene uuid
   */
  Uuid startingScene;
};

} // namespace liquid::runtime
