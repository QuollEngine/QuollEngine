#include "liquid/core/Base.h"
#include "DeviceStats.h"

namespace liquid::rhi {

void DeviceStats::addDrawCall(size_t primitiveCount) {
  mDrawCallsCount++;
  mDrawnPrimitivesCount += primitiveCount;
}

void DeviceStats::resetDrawCalls() {
  mDrawCallsCount = 0;
  mDrawnPrimitivesCount = 0;
}

} // namespace liquid::rhi
