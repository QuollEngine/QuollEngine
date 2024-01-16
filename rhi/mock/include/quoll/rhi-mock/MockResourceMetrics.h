#pragma once

#include "quoll/rhi/NativeResourceMetrics.h"

namespace quoll::rhi {

class MockResourceMetrics : public NativeResourceMetrics {
public:
  usize getTotalBufferSize() const override;

  usize getBuffersCount() const override;

  usize getTexturesCount() const override;

  usize getDescriptorsCount() const override;
};

} // namespace quoll::rhi
