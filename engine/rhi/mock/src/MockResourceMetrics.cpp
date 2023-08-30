#include "quoll/core/Base.h"
#include "MockResourceMetrics.h"

namespace quoll::rhi {

size_t MockResourceMetrics::getTotalBufferSize() const { return 0; }

size_t MockResourceMetrics::getBuffersCount() const { return 0; }

size_t MockResourceMetrics::getTexturesCount() const { return 0; }

size_t MockResourceMetrics::getDescriptorsCount() const { return 0; }

} // namespace quoll::rhi
