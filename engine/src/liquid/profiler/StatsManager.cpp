#include "liquid/core/Base.h"
#include "StatsManager.h"

namespace liquid {

void StatsManager::addDrawCall(size_t primitiveCount) {
  mDrawCallsCount++;
  mDrawnPrimitivesCount += primitiveCount;
}

void StatsManager::resetDrawCalls() {
  mDrawCallsCount = 0;
  mDrawnPrimitivesCount = 0;
}

void StatsManager::addTexture(size_t size) {
  mAllocatedTexturesSize += size;
  mAllocatedTexturesCount++;
}

void StatsManager::removeTexture(size_t size) {
  mAllocatedTexturesSize -= size;
  mAllocatedTexturesCount--;
}

void StatsManager::addBuffer(size_t size) {
  mAllocatedBuffersSize += size;
  mAllocatedBuffersCount++;
}

void StatsManager::removeBuffer(size_t size) {
  mAllocatedBuffersSize -= size;
  mAllocatedBuffersCount--;
}

void StatsManager::collectFPS(uint32_t fps) { mFps = fps; }

} // namespace liquid
