#include "quoll/core/Base.h"
#include "MockResourceMetrics.h"

namespace quoll::rhi {

usize MockResourceMetrics::getTotalBufferSize() const { return 0; }

usize MockResourceMetrics::getBuffersCount() const { return 0; }

usize MockResourceMetrics::getTexturesCount() const { return 0; }

usize MockResourceMetrics::getDescriptorsCount() const { return 0; }

} // namespace quoll::rhi
