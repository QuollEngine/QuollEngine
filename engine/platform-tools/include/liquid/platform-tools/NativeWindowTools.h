#pragma once

#include <GLFW/glfw3.h>

namespace liquid::platform_tools {

/**
 * @brief Native window tools
 *
 * Abstracts away specific window functionality
 * for each platform. If a feature does not exist
 * in a platform, calling the feature will be
 * noop.
 */
class NativeWindowTools {
public:
  /**
   * @brief Enable dark mode
   *
   * @param window GLFW window
   */
  static void enableDarkMode(GLFWwindow *window);
};

} // namespace liquid::platform_tools
