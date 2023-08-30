#pragma once

namespace quoll::runtime {

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

} // namespace quoll::runtime
