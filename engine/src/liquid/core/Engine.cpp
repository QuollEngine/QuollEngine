#include "liquid/core/Base.h"
#include "liquid/logger/StreamTransport.h"
#include "Engine.h"

namespace liquid {

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
Engine Engine::engine;

Engine::Engine() { resetLoggers(); }

void Engine::setPath(const Path &path) {
  engine.mEnginePath = path;
  engine.mAssetsPath = engine.mEnginePath / "assets";
}

const Path Engine::getShadersPath() { return engine.mAssetsPath / "shaders"; }

const Path Engine::getFontsPath() { return engine.mAssetsPath / "fonts"; }

const Path Engine::getEnginePath() { return engine.mEnginePath; }

Logger &Engine::getLogger() { return engine.mSystemLogger; }

Logger &Engine::getUserLogger() { return engine.mUserLogger; }

void Engine::resetLoggers() {
  engine.mSystemLogger.setTransport(createStreamTransport(std::cout));
  engine.mUserLogger.setTransport(createStreamTransport(std::cout));
}

} // namespace liquid
