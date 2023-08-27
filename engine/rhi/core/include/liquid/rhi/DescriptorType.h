#pragma once

namespace liquid::rhi {

enum class DescriptorType {
  UniformBuffer,
  UniformBufferDynamic,
  StorageBuffer,
  SampledImage,
  StorageImage,
  Sampler,
  None
};

} // namespace liquid::rhi
