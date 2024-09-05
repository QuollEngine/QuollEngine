#pragma once

#include "quoll/logger/Logger.h"
#include "quoll/threads/ThreadPool.h"

namespace quoll {

struct EngineDescription {
  Path path;

  u32 numThreads = 0;
};

/**
 * Singleton with values that are set once
 * during application load and read from various
 * modules
 */
class Engine {
public:
  static void create(EngineDescription description);

  static void destroy();

  static const Path getShadersPath();

  static const Path getFontsPath();

  static const Path getEnginePath();

  static Logger &getLogger();

  static Logger &getUserLogger();

  static void resetLoggers();

  static ThreadPool &getThreadPool();

private:
  Engine(EngineDescription description);

private:
  Path mEnginePath;
  Logger mSystemLogger;
  Logger mUserLogger;
  ThreadPool mThreadPool;
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
