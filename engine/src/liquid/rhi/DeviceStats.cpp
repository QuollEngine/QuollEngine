#include "liquid/core/Base.h"
#include "DeviceStats.h"

namespace liquid::rhi {

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

} // namespace liquid::rhi
