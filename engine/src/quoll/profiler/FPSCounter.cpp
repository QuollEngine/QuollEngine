#include "quoll/core/Base.h"
#include "FPSCounter.h"

namespace quoll {

void FPSCounter::collectFPS(u32 fps) { mFps = fps; }

} // namespace quoll
