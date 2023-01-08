#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"
#include "VulkanError.h"
#include "VulkanLog.h"

namespace liquid::rhi {

VulkanDescriptorPool::VulkanDescriptorPool(
    VulkanDeviceObject &device, VulkanResourceRegistry &registry,
    VulkanPipelineLayoutCache &pipelineLayoutCache)
    : mDevice(device), mRegistry(registry),
      mPipelineLayoutCache(pipelineLayoutCache) {
  createDescriptorPool();
}

VulkanDescriptorPool::~VulkanDescriptorPool() {
  if (mDescriptorPool) {
    vkDestroyDescriptorPool(mDevice, mDescriptorPool, nullptr);
    LOG_DEBUG_VK("Descriptor pool destroyed", mDescriptorPool);
  }
}

void VulkanDescriptorPool::createDescriptorPool() {
  static constexpr uint32_t NumUniformBuffers = 15000;
  static constexpr uint32_t NumStorageBuffers = 10;
  static constexpr uint32_t NumSamplers = 1000;
  static constexpr uint32_t NumDescriptors = 30000;
  static constexpr uint32_t MaxTextureDescriptors = 8;

  static constexpr uint32_t MaxImageSamplers =
      MaxTextureDescriptors * NumSamplers;

  std::array<VkDescriptorPoolSize, 3> poolSizes{
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                           NumUniformBuffers},
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                           NumStorageBuffers},
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                           MaxImageSamplers}};

  VkDescriptorPoolCreateInfo descriptorPoolInfo{};
  descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolInfo.pNext = nullptr;
  descriptorPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
  descriptorPoolInfo.maxSets = NumDescriptors;
  descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  descriptorPoolInfo.pPoolSizes = poolSizes.data();

  checkForVulkanError(vkCreateDescriptorPool(mDevice, &descriptorPoolInfo,
                                             nullptr, &mDescriptorPool),
                      "Failed to create descriptor pool");

  LOG_DEBUG_VK("Descriptor pool created. Samplers: "
                   << MaxImageSamplers << "; UB: " << NumUniformBuffers
                   << "; SB: " << NumStorageBuffers,
               mDescriptorPool);
}

Descriptor
VulkanDescriptorPool::createDescriptor(DescriptorLayoutHandle layout) {
  const auto &description =
      mPipelineLayoutCache.getDescriptorLayoutDescription(layout);
  VkDescriptorSetLayout vulkanLayout =
      mPipelineLayoutCache.getVulkanDescriptorSetLayout(layout);

  VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
  descriptorSetAllocateInfo.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  descriptorSetAllocateInfo.pNext = nullptr;
  descriptorSetAllocateInfo.descriptorPool = mDescriptorPool;
  descriptorSetAllocateInfo.pSetLayouts = &vulkanLayout;
  descriptorSetAllocateInfo.descriptorSetCount = 1;

  VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
  checkForVulkanError(vkAllocateDescriptorSets(
                          mDevice, &descriptorSetAllocateInfo, &descriptorSet),
                      "Failed to allocate descriptor set");

  mDescriptorSets.push_back(descriptorSet);

  return Descriptor(new VulkanDescriptorSet(mDevice, mRegistry, descriptorSet),
                    static_cast<DescriptorHandle>(mDescriptorSets.size()));
}

VkDescriptorSet
VulkanDescriptorPool::createDescriptor(VkDescriptorSetLayout layout) {
  VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

  VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
  descriptorSetAllocateInfo.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  descriptorSetAllocateInfo.pNext = nullptr;
  descriptorSetAllocateInfo.descriptorPool = mDescriptorPool;
  descriptorSetAllocateInfo.pSetLayouts = &layout;
  descriptorSetAllocateInfo.descriptorSetCount = 1;

  checkForVulkanError(vkAllocateDescriptorSets(
                          mDevice, &descriptorSetAllocateInfo, &descriptorSet),
                      "Failed to allocate descriptor set");

  mDescriptorSets.push_back(descriptorSet);

  return descriptorSet;
}

void VulkanDescriptorPool::reset() {
  vkResetDescriptorPool(mDevice, mDescriptorPool, 0);
  mDescriptorSets.clear();
}

} // namespace liquid::rhi
