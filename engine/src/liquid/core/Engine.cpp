#include "liquid/core/Base.h"
#include "Engine.h"

namespace liquid {

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
Engine Engine::engine;

void Engine::setPath(const Path &path) {
  engine.mEnginePath = path;
  engine.mAssetsPath = engine.mEnginePath / "assets";
}

const Path Engine::getShadersPath() { return engine.mAssetsPath / "shaders"; }

const Path Engine::getFontsPath() { return engine.mAssetsPath / "fonts"; }

const Path Engine::getEnginePath() { return engine.mEnginePath; }

} // namespace liquid
