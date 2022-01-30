#include "liquid/core/Base.h"
#include "StatsManager.h"

namespace liquid {

void StatsManager::addDrawCall(size_t primitiveCount) {
  drawCallsCount++;
  drawnPrimitivesCount += primitiveCount;
}

void StatsManager::resetDrawCalls() {
  drawCallsCount = 0;
  drawnPrimitivesCount = 0;
}

void StatsManager::addTexture(size_t size) {
  allocatedTexturesSize += size;
  allocatedTexturesCount++;
}

void StatsManager::removeTexture(size_t size) {
  allocatedTexturesSize -= size;
  allocatedTexturesCount--;
}

void StatsManager::addBuffer(size_t size) {
  allocatedBuffersSize += size;
  allocatedBuffersCount++;
}

void StatsManager::removeBuffer(size_t size) {
  allocatedBuffersSize -= size;
  allocatedBuffersCount--;
}

void StatsManager::collectFPS(uint32_t fps_) { fps = fps_; }

} // namespace liquid
