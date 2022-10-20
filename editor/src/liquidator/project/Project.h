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
   * Path to scene directory
   */
  liquid::Path scenePath;

  /**
   * Path to settings directory
   */
  liquid::Path settingsPath;
};

} // namespace liquidator
