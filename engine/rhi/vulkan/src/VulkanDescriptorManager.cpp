#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include <vulkan/vulkan.hpp>

#include "VulkanDescriptorManager.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanMapping.h"
#include "VulkanError.h"

#include "VulkanLog.h"

namespace liquid::rhi {

VulkanDescriptorManager::VulkanDescriptorManager(
    VulkanDeviceObject &device, const VulkanResourceRegistry &registry)
    : mDevice(device), mRegistry(registry) {
  createDescriptorPool();
}

VulkanDescriptorManager::~VulkanDescriptorManager() {
  if (mDescriptorPool) {
    vkDestroyDescriptorPool(mDevice, mDescriptorPool, nullptr);
    LOG_DEBUG_VK("Descriptor pool destroyed", mDescriptorPool);
  }
}

VkDescriptorSet
VulkanDescriptorManager::getOrCreateDescriptor(const Descriptor &descriptor,
                                               VkDescriptorSetLayout layout) {
  const String &hash = createHash(descriptor, layout);
  const auto &found = mDescriptorCache.find(hash);

  if (found == mDescriptorCache.end()) {
    VkDescriptorSet set = createDescriptorSet(descriptor, layout);
    mDescriptorCache.insert({hash, set});
    LOG_DEBUG_VK("Descriptor set allocated: \"" << descriptor.getHashCode()
                                                << "\"",
                 set);
    return set;
  }
  return (*found).second;
}

VkDescriptorSet
VulkanDescriptorManager::createDescriptorSet(const Descriptor &descriptor,
                                             VkDescriptorSetLayout layout) {
  VkDescriptorSet descriptorSet = allocateDescriptorSet(layout);

  std::vector<VkWriteDescriptorSet> writes;
  std::vector<std::vector<VkDescriptorImageInfo>> imageInfos;
  std::vector<VkDescriptorBufferInfo> bufferInfos;

  imageInfos.reserve(descriptor.getBindings().size());
  bufferInfos.reserve(descriptor.getBindings().size());

  for (const auto &binding : descriptor.getBindings()) {
    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;
    write.dstBinding = binding.first;
    write.dstSet = descriptorSet;
    write.dstArrayElement = 0;
    write.pBufferInfo = nullptr;
    write.pImageInfo = nullptr;
    write.pTexelBufferView = nullptr;
    write.descriptorType =
        VulkanMapping::getDescriptorType(binding.second.type);

    if (binding.second.type == DescriptorType::UniformBuffer ||
        binding.second.type == DescriptorType::StorageBuffer) {
      const auto &vkBuffer = mRegistry.getBuffers().at(
          std::get<BufferHandle>(binding.second.data));
      bufferInfos.push_back(VkDescriptorBufferInfo{vkBuffer->getBuffer(), 0,
                                                   vkBuffer->getSize()});

      const auto &bufferObj = bufferInfos.at(bufferInfos.size() - 1);
      write.pBufferInfo = &bufferObj;
      write.descriptorCount = 1;
      writes.push_back(write);
    } else if (binding.second.type == DescriptorType::CombinedImageSampler) {
      imageInfos.push_back({});
      auto &imageInfoObjects = imageInfos.at(imageInfos.size() - 1);

      const auto &textures =
          std::get<std::vector<TextureHandle>>(binding.second.data);
      for (const auto tex : textures) {
        if (!rhi::isHandleValid(tex))
          continue;

        const auto &native = mRegistry.getTextures().at(tex);

        imageInfoObjects.push_back({native->getSampler(),
                                    native->getImageView(),
                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
      }

      if (!imageInfoObjects.empty()) {
        write.pImageInfo = imageInfoObjects.data();
        write.descriptorCount = static_cast<uint32_t>(imageInfoObjects.size());
        writes.push_back(write);
      }
    }
  }

  vkUpdateDescriptorSets(mDevice, static_cast<uint32_t>(writes.size()),
                         writes.data(), 0, nullptr);

  return descriptorSet;
}

VkDescriptorSet
VulkanDescriptorManager::allocateDescriptorSet(VkDescriptorSetLayout layout) {
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

  return descriptorSet;
}

void VulkanDescriptorManager::createDescriptorPool() {
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
  descriptorPoolInfo.flags = 0;
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

String VulkanDescriptorManager::createHash(const Descriptor &descriptor,
                                           VkDescriptorSetLayout layout) {
  std::stringstream ss;
  ss << descriptor.getHashCode() << layout;
  return ss.str();
}

} // namespace liquid::rhi
