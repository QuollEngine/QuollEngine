#include "liquid/core/Base.h"
#include "FPSCounter.h"

namespace quoll {

void FPSCounter::collectFPS(uint32_t fps) { mFps = fps; }

} // namespace quoll
