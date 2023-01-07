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
    VulkanDeviceObject &device, const VulkanResourceRegistry &registry,
    VulkanDescriptorPool &descriptorPool)
    : mDevice(device), mRegistry(registry), mDescriptorPool(descriptorPool) {}

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

void VulkanDescriptorManager::clear() { mDescriptorCache.clear(); }

VkDescriptorSet
VulkanDescriptorManager::createDescriptorSet(const Descriptor &descriptor,
                                             VkDescriptorSetLayout layout) {
  VkDescriptorSet descriptorSet = mDescriptorPool.createDescriptor(layout);

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

String VulkanDescriptorManager::createHash(const Descriptor &descriptor,
                                           VkDescriptorSetLayout layout) {
  std::stringstream ss;
  ss << descriptor.getHashCode() << layout;
  return ss.str();
}

} // namespace liquid::rhi
