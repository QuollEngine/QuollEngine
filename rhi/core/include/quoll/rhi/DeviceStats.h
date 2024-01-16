#pragma once

#include "NativeResourceMetrics.h"

namespace quoll::rhi {

class DeviceStats {
public:
  DeviceStats(NativeResourceMetrics *resourceMetrics);

  void addDrawCall(usize primitiveCount);

  void resetCalls();

  void addCommandCall();

  inline u32 getDrawCallsCount() const { return mDrawCallsCount; }

  inline usize getDrawnPrimitivesCount() const { return mDrawnPrimitivesCount; }

  inline u32 getCommandCallsCount() const { return mCommandCallsCount; }

  inline const NativeResourceMetrics *getResourceMetrics() const {
    return mResourceMetrics;
  }

private:
  u32 mDrawCallsCount = 0;
  usize mDrawnPrimitivesCount = 0;
  u32 mCommandCallsCount = 0;

  NativeResourceMetrics *mResourceMetrics;
};

} // namespace quoll::rhi
