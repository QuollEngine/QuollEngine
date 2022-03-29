#include "liquid/core/Base.h"
#include "FPSCounter.h"

namespace liquid {

void FPSCounter::collectFPS(uint32_t fps) { mFps = fps; }

} // namespace liquid
