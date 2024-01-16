#pragma once

namespace quoll::rhi {

class NativeResourceMetrics {
public:
  virtual usize getTotalBufferSize() const = 0;

  virtual usize getBuffersCount() const = 0;

  virtual usize getTexturesCount() const = 0;

  virtual usize getDescriptorsCount() const = 0;
};

} // namespace quoll::rhi
