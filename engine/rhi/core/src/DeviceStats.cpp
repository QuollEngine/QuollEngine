#include "quoll/core/Base.h"
#include "DeviceStats.h"

namespace quoll::rhi {

DeviceStats::DeviceStats(NativeResourceMetrics *resourceMetrics)
    : mResourceMetrics(resourceMetrics) {}

void DeviceStats::addDrawCall(size_t primitiveCount) {
  mDrawCallsCount++;
  mDrawnPrimitivesCount += primitiveCount;
  mCommandCallsCount++;
}

void DeviceStats::resetCalls() {
  mDrawCallsCount = 0;
  mDrawnPrimitivesCount = 0;
  mCommandCallsCount = 0;
}

void DeviceStats::addCommandCall() { mCommandCallsCount++; }

} // namespace quoll::rhi
