#include "quoll/core/Base.h"
#include "GpuSpan.h"
#include "MetricsCollector.h"

namespace quoll {

GpuSpan::GpuSpan(MetricsCollector &collector, usize spanIndex, usize startMark,
                 usize endMark)
    : mMetricsCollector(collector), mSpanIndex(spanIndex),
      mStartMark(startMark), mEndMark(endMark) {}

void GpuSpan::begin(rhi::RenderCommandList &commandList) {
  mMetricsCollector.mQueries.at(mStartMark) =
      static_cast<u32>(mMetricsCollector.mRecordedTimestamps.size());

  mMetricsCollector.mRecordedTimestamps.push_back(0);

  commandList.writeTimestamp(mMetricsCollector.getQueryForMark(mStartMark),
                             rhi::PipelineStage::PipeTop);
}

void GpuSpan::end(rhi::RenderCommandList &commandList) {
  mMetricsCollector.mQueries.at(mEndMark) =
      static_cast<u32>(mMetricsCollector.mRecordedTimestamps.size());

  mMetricsCollector.mRecordedTimestamps.push_back(0);
  mMetricsCollector.mRecordedSpans.push_back(mSpanIndex);

  commandList.writeTimestamp(mMetricsCollector.getQueryForMark(mEndMark),
                             rhi::PipelineStage::PipeBottom);
}

} // namespace quoll
