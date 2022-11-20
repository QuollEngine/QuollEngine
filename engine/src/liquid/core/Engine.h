#pragma once

#include "liquid/logger/Logger.h"

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
   * @brief Set path for engine data
   *
   * @param path Engine path
   */
  static void setPath(const Path &path);

  /**
   * @brief Get path for engine shaders
   *
   * @return Engine shaders path
   */
  static const Path getShadersPath();

  /**
   * @brief Get path for engine fonts
   *
   * @return Engine fonts path
   */
  static const Path getFontsPath();

  /**
   * @brief Get engine path
   *
   * @return Engine path
   */
  static const Path getEnginePath();

  /**
   * @brief Get engine logger
   *
   * @return Engine logger
   */
  static Logger &getLogger();

private:
  /**
   * @brief Create engine
   *
   * This constructor is private in order to disallow
   * creating individual engine object
   */
  Engine();

private:
  Path mAssetsPath;
  Path mEnginePath;

  Logger mLogger;
};

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#if defined(LIQUID_DEBUG)
#define LOG_DEBUG(stream) Engine::getLogger().log(LogSeverity::Debug) << stream
#else
#define LOG_DEBUG(_)                                                           \
  {}
#endif
// NOLINTEND(cppcoreguidelines-macro-usage)

} // namespace liquid
