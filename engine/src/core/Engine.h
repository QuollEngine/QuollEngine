#pragma once

#include "core/Base.h"

namespace liquid {

/**
 * @brief Global engine singleton
 *
 * This singleton is used to store only
 * global read only variables that are set only
 * once by the application and read from multiple
 * modules.
 */
class Engine {
private:
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
  static Engine engine;

public:
  /**
   * @brief Set assets path for engine
   *
   * @param path Assets path
   */
  static void setAssetsPath(const String &path);

  /**
   * @brief Get assets path for engine
   *
   * @return Assets path
   */
  static const String &getAssetsPath();

private:
  /**
   * @brief Create engine
   *
   * This constructor is private in order to disallow
   * creating individual engine object
   */
  Engine() = default;

private:
  String assetsPath;
};

} // namespace liquid
