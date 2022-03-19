#include "liquid/core/Base.h"
#include "Engine.h"

namespace liquid {

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
Engine Engine::engine;

void Engine::setAssetsPath(const String &path) { engine.mAssetsPath = path; }

const String &Engine::getAssetsPath() { return engine.mAssetsPath; }

} // namespace liquid
