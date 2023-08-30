#pragma once

namespace quoll::rhi {

enum class DescriptorType {
  UniformBuffer,
  UniformBufferDynamic,
  StorageBuffer,
  SampledImage,
  StorageImage,
  Sampler,
  None
};

} // namespace quoll::rhi
