#pragma once

namespace liquid::editor {

/**
 * @brief Project data
 */
struct Project {
  /**
   * Project name
   */
  String name;

  /**
   * Project version
   */
  String version;

  /**
   * Path to assets directory
   */
  Path assetsPath;

  /**
   * @brief Path to assets cache directory
   */
  Path assetsCachePath;

  /**
   * Path to scenes directory
   */
  Path scenesPath;

  /**
   * Path to settings directory
   */
  Path settingsPath;
};

} // namespace liquid::editor
