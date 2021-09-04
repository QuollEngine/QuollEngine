#include "Engine.h"

namespace liquid {

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
Engine Engine::engine;

void Engine::setAssetsPath(const String &path) { engine.assetsPath = path; }

const String &Engine::getAssetsPath() { return engine.assetsPath; }

} // namespace liquid
