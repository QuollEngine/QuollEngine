#pragma once

#include "quoll/logger/Logger.h"

namespace quoll {

/**
 * Singleton with values that are set once
 * during application load and read from various
 * modules
 */
class Engine {
private:
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
  static Engine engine;

public:
  static void setEnginePath(const Path &path);

  static const Path getShadersPath();

  static const Path getFontsPath();

  static const Path getEnginePath();

  static Logger &getLogger();

  static Logger &getUserLogger();

  static void resetLoggers();

private:
  Engine();

private:
  Path mAssetsPath;
  Path mEnginePath;

  Logger mSystemLogger;

  Logger mUserLogger;
};

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#if defined(QUOLL_DEBUG)
#define LOG_DEBUG(stream) quoll::Engine::getLogger().debug() << stream
#else
#define LOG_DEBUG(_)                                                           \
  {}
#endif
// NOLINTEND(cppcoreguidelines-macro-usage)

} // namespace quoll
