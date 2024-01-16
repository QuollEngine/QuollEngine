#pragma once

#include "quoll/rhi/NativeDescriptor.h"
#include "VulkanDeviceObject.h"
#include "VulkanResourceRegistry.h"

namespace quoll::rhi {

class VulkanDescriptorSet : public NativeDescriptor {
public:
  VulkanDescriptorSet(VulkanDeviceObject &device,
                      const VulkanResourceRegistry &registry,
                      VkDescriptorSet descriptorSet);

  void write(u32 binding, std::span<TextureHandle> textures,
             DescriptorType type, u32 start) override;

  void write(u32 binding, std::span<SamplerHandle> samplers,
             u32 start) override;

  void write(u32 binding, std::span<BufferHandle> buffers, DescriptorType type,
             u32 start) override;

  void write(u32 binding, std::span<DescriptorBufferInfo> bufferInfos,
             DescriptorType type, u32 start) override;

private:
  void write(u32 binding, u32 start, usize descriptorCount,
             VkDescriptorType type, const VkDescriptorImageInfo *imageInfos,
             VkDescriptorBufferInfo *bufferInfos);

private:
  VulkanDeviceObject &mDevice;
  const VulkanResourceRegistry &mRegistry;
  VkDescriptorSet mDescriptorSet = VK_NULL_HANDLE;
};

} // namespace quoll::rhi
