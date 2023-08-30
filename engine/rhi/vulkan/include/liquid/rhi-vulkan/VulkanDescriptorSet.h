#pragma once

#include "liquid/rhi/NativeDescriptor.h"

#include "VulkanDeviceObject.h"
#include "VulkanResourceRegistry.h"

namespace quoll::rhi {

/**
 * @brief Vulkan descriptor set
 */
class VulkanDescriptorSet : public NativeDescriptor {
public:
  /**
   * @brief Create Vulkan descriptor set
   *
   * @param device Vulkan device
   * @param registry Vulkan resource registry
   * @param descriptorSet Vulkan descriptor set
   */
  VulkanDescriptorSet(VulkanDeviceObject &device,
                      const VulkanResourceRegistry &registry,
                      VkDescriptorSet descriptorSet);

  /**
   * @brief Write textures
   *
   * @param binding Binding number
   * @param textures Textures
   * @param type Descriptor type
   * @param start Starting index
   */
  void write(uint32_t binding, std::span<TextureHandle> textures,
             DescriptorType type, uint32_t start) override;

  /**
   * @brief Bind sampler descriptors
   *
   * @param binding Binding number
   * @param samplers Samplers
   * @param start Starting index
   */
  void write(uint32_t binding, std::span<SamplerHandle> samplers,
             uint32_t start) override;

  /**
   * @brief Write buffers
   *
   * @param binding Binding number
   * @param buffers Buffers
   * @param type Descriptor type
   * @param start Starting index
   */
  void write(uint32_t binding, std::span<BufferHandle> buffers,
             DescriptorType type, uint32_t start) override;

  /**
   * @brief Bind buffer descriptors
   *
   * @param binding Binding number
   * @param bufferInfos Buffer infos
   * @param type Descriptor type
   * @param start Starting index
   */
  void write(uint32_t binding, std::span<DescriptorBufferInfo> bufferInfos,
             DescriptorType type, uint32_t start) override;

private:
  /**
   * @brief Write descriptor set
   *
   * @param binding Binding number
   * @param start Starting index
   * @param descriptorCount Descriptor count
   * @param type Descriptor type
   * @param imageInfos Image infos
   * @param bufferInfos Buffer infos
   */
  void write(uint32_t binding, uint32_t start, size_t descriptorCount,
             VkDescriptorType type, const VkDescriptorImageInfo *imageInfos,
             VkDescriptorBufferInfo *bufferInfos);

private:
  VulkanDeviceObject &mDevice;
  const VulkanResourceRegistry &mRegistry;
  VkDescriptorSet mDescriptorSet = VK_NULL_HANDLE;
};

} // namespace quoll::rhi
