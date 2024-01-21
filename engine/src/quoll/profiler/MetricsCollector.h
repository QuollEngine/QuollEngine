#pragma once

#include "quoll/core/SparseSet.h"
#include "quoll/rhi/RenderDevice.h"
#include "GpuSpan.h"

namespace quoll {

class MetricsCollector {
  friend class GpuSpan;

private:
  struct GpuSpanData {
    String label;
    usize markStart;
    usize markEnd;
  };

  struct Metric {
    String label;
    f32 value;
  };

public:
  GpuSpan createGpuSpan(String label);

  void deleteGpuSpan(GpuSpan span);

  void getResults(rhi::RenderDevice *device);

  void markForCollection();

  std::vector<Metric> measure(f32 converter);

  inline u32 getQueryForMark(usize mark) {
    QuollAssert(mQueries.contains(mark), "Query does not exist");
    return mQueries.at(mark);
  }

  inline const GpuSpanData &getGpuSpanData(usize index) const {
    QuollAssert(index < mGpuSpans.size(), "Span does not exist");
    return mGpuSpans.at(index);
  }

  inline usize getNumQueries() const { return mQueries.size(); }

  inline usize getNumCollectedTimestamps() {
    return mCollectedTimestamps.size();
  }

private:
  usize createMark();

private:
  SparseSet<GpuSpanData> mGpuSpans;
  SparseSet<u32> mQueries;
  std::vector<u64> mRecordedTimestamps;
  std::vector<u64> mCollectedTimestamps;

  std::vector<usize> mRecordedSpans;
  std::vector<usize> mCollectedSpans;

  bool mCollect = false;
};

} // namespace quoll
