#include "quoll/core/Base.h"
#include "quoll/core/Profiler.h"
#include "MetricsCollector.h"

namespace quoll {

static constexpr usize TimestampsInitialSize = 100;
static constexpr usize TimestampsPoolSize = 500;

GpuSpan MetricsCollector::createGpuSpan(String label) {
  mRecordedTimestamps.reserve(TimestampsInitialSize);

  auto start = createMark();
  auto end = createMark();

  auto index =
      mGpuSpans.insert({.label = label, .markStart = start, .markEnd = end});

  return GpuSpan(*this, index, start, end);
}

void MetricsCollector::deleteGpuSpan(GpuSpan span) {
  const auto &spanData = mGpuSpans.at(span.getIndex());

  mQueries.erase(spanData.markStart);
  mQueries.erase(spanData.markEnd);
  mGpuSpans.erase(span.getIndex());
}

void MetricsCollector::getResults(rhi::RenderDevice *device) {
  if (!mCollect) {
    mRecordedTimestamps.clear();
    mRecordedSpans.clear();
    return;
  }

  QUOLL_PROFILE_EVENT("MetricsCollector::getResults");
  device->collectTimestamps(mRecordedTimestamps);

  mCollectedTimestamps.reserve(mRecordedTimestamps.size());
  mCollectedTimestamps.clear();
  for (auto ts : mRecordedTimestamps) {
    mCollectedTimestamps.push_back(ts);
  }
  mRecordedTimestamps.clear();

  mCollectedSpans.reserve(mRecordedSpans.size());
  mCollectedSpans.clear();
  for (auto spanIndex : mRecordedSpans) {
    mCollectedSpans.push_back(spanIndex);
  }
  mRecordedSpans.clear();

  mCollect = false;
}

void MetricsCollector::markForCollection() { mCollect = true; }

std::vector<MetricsCollector::Metric> MetricsCollector::measure(f32 converter) {
  std::vector<Metric> metrics;
  metrics.reserve(mGpuSpans.size());
  for (auto spanIndex : mCollectedSpans) {
    const auto &span = mGpuSpans.at(spanIndex);

    const f32 value =
        static_cast<f32>(mCollectedTimestamps.at(mQueries.at(span.markEnd)) -
                         mCollectedTimestamps.at(mQueries.at(span.markStart))) *
        converter;

    metrics.push_back({.label = span.label, .value = value});
  }

  return metrics;
}

usize MetricsCollector::createMark() {
  auto mark = mQueries.insert(0);

  mRecordedTimestamps.reserve(mQueries.size());

  return mark;
}

} // namespace quoll
