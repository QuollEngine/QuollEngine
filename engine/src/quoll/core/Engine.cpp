#include "quoll/core/Base.h"
#include "quoll/logger/StreamTransport.h"
#include "Engine.h"

namespace quoll {

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
Engine *sEngine = nullptr;

void Engine::create(EngineDescription description) {
  sEngine = new Engine(description);
}

void Engine::destroy() { delete sEngine; }

Engine::Engine(EngineDescription description)
    : mEnginePath(description.path), mThreadPool(description.numThreads) {
  resetLoggers();
}

const Path Engine::getShadersPath() {
  return sEngine->mEnginePath / "assets" / "shaders";
}

const Path Engine::getFontsPath() {
  return sEngine->mEnginePath / "assets" / "fonts";
}

const Path Engine::getEnginePath() { return sEngine->mEnginePath; }

Logger &Engine::getLogger() { return sEngine->mSystemLogger; }

Logger &Engine::getUserLogger() { return sEngine->mUserLogger; }

void Engine::resetLoggers() {
  sEngine->mSystemLogger.setTransport(createStreamTransport(std::cout));
  sEngine->mUserLogger.setTransport(createStreamTransport(std::cout));
}

} // namespace quoll
