#pragma once

namespace liquid::platform {

/**
 * @brief Platform file opener
 */
class FileOpener {
public:
  /**
   * @brief Open file
   *
   * @param path Path to file
   */
  static void openFile(const Path &path);
};

} // namespace liquid::platform
