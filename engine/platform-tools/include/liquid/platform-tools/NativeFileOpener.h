#pragma once

namespace liquid::platform_tools {

/**
 * @brief Native file opener
 *
 * Abstracts away platform specific
 * default file opener
 */
class NativeFileOpener {
public:
  /**
   * @brief Open file
   *
   * @param path Path to file
   */
  void openFile(const Path &path);
};

} // namespace liquid::platform_tools
