#pragma once

namespace liquidator {

/**
 * @brief Project data
 */
struct Project {
  /**
   * Project name
   */
  liquid::String name;

  /**
   * Project version
   */
  liquid::String version;

  /**
   * Path to assets directory
   */
  liquid::Path assetsPath;

  /**
   * Path to scenes directory
   */
  liquid::Path scenesPath;

  /**
   * Path to settings directory
   */
  liquid::Path settingsPath;
};

} // namespace liquidator
