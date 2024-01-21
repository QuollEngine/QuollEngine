#pragma once

#include "quoll/rhi/RenderCommandList.h"

namespace quoll {

class MetricsCollector;

class GpuSpan {
public:
  GpuSpan(MetricsCollector &collector, usize spanIndex, usize startMark,
          usize endMark);

  void begin(rhi::RenderCommandList &commandList);

  void end(rhi::RenderCommandList &commandList);

  inline usize getIndex() const { return mSpanIndex; }

  inline usize getStartMark() const { return mStartMark; }

  inline usize getEndMark() const { return mEndMark; }

private:
  MetricsCollector &mMetricsCollector;
  usize mSpanIndex;
  usize mStartMark;
  usize mEndMark;
};

} // namespace quoll