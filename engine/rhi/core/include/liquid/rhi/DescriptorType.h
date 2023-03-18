#pragma once

namespace liquid::rhi {

enum class DescriptorType {
  UniformBuffer,
  UniformBufferDynamic,
  StorageBuffer,
  CombinedImageSampler,
  StorageImage
};

} // namespace liquid::rhi
