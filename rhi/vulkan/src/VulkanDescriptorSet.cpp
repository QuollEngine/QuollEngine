#include "quoll/core/Base.h"

#include "VulkanDescriptorSet.h"
#include "VulkanMapping.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanSampler.h"

namespace quoll::rhi {

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDeviceObject &device,
                                         const VulkanResourceRegistry &registry,
                                         VkDescriptorSet descriptorSet)
    : mDevice(device), mRegistry(registry), mDescriptorSet(descriptorSet) {}

void VulkanDescriptorSet::write(u32 binding, std::span<TextureHandle> textures,
                                DescriptorType type, u32 start) {
  std::vector<VkDescriptorImageInfo> imageInfos(textures.size(),
                                                VkDescriptorImageInfo{});

  for (usize i = 0; i < textures.size(); ++i) {
    const auto &vkTexture = mRegistry.getTextures().at(textures[i]);
    imageInfos.at(i).imageLayout =
        type == DescriptorType::StorageImage
            ? VK_IMAGE_LAYOUT_GENERAL
            : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfos.at(i).imageView = vkTexture->getImageView();
    imageInfos.at(i).sampler = VK_NULL_HANDLE;
  }

  write(binding, start, imageInfos.size(),
        VulkanMapping::getDescriptorType(type), imageInfos.data(), nullptr);
}

void VulkanDescriptorSet::write(u32 binding, std::span<SamplerHandle> samplers,
                                u32 start) {
  std::vector<VkDescriptorImageInfo> samplerInfos{samplers.size(),
                                                  VkDescriptorImageInfo{}};

  for (usize i = 0; i < samplers.size(); ++i) {
    const auto &vkSampler = mRegistry.getSamplers().at(samplers[i]);
    samplerInfos.at(i).sampler = vkSampler->getSampler();
    samplerInfos.at(i).imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    samplerInfos.at(i).imageView = VK_NULL_HANDLE;
  }

  write(binding, start, samplerInfos.size(), VK_DESCRIPTOR_TYPE_SAMPLER,
        samplerInfos.data(), nullptr);
}

void VulkanDescriptorSet::write(u32 binding, std::span<BufferHandle> buffers,
                                DescriptorType type, u32 start) {
  std::vector<VkDescriptorBufferInfo> bufferInfos(buffers.size(),
                                                  VkDescriptorBufferInfo{});

  for (usize i = 0; i < buffers.size(); ++i) {
    const auto &vkBuffer = mRegistry.getBuffers().at(buffers[i]);
    bufferInfos.at(i).buffer = vkBuffer->getBuffer();
    bufferInfos.at(i).offset = 0;
    bufferInfos.at(i).range = vkBuffer->getSize();
  }

  write(binding, start, bufferInfos.size(),
        VulkanMapping::getDescriptorType(type), nullptr, bufferInfos.data());
}

void VulkanDescriptorSet::write(u32 binding,
                                std::span<DescriptorBufferInfo> bufferInfos,
                                DescriptorType type, u32 start) {
  std::vector<VkDescriptorBufferInfo> vkBufferInfos(bufferInfos.size(),
                                                    VkDescriptorBufferInfo{});

  for (usize i = 0; i < bufferInfos.size(); ++i) {
    const auto &vkBuffer = mRegistry.getBuffers().at(bufferInfos[i].buffer);
    vkBufferInfos.at(i).buffer = vkBuffer->getBuffer();
    vkBufferInfos.at(i).offset = bufferInfos[i].offset;
    vkBufferInfos.at(i).range = bufferInfos[i].range;
  }

  write(binding, start, bufferInfos.size(),
        VulkanMapping::getDescriptorType(type), nullptr, vkBufferInfos.data());
}

void VulkanDescriptorSet::write(u32 binding, u32 start, usize descriptorCount,
                                VkDescriptorType type,
                                const VkDescriptorImageInfo *imageInfos,
                                VkDescriptorBufferInfo *bufferInfos) {
  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.pNext = nullptr;
  write.dstBinding = binding;
  write.dstSet = mDescriptorSet;
  write.descriptorCount = static_cast<u32>(descriptorCount);
  write.dstArrayElement = start;
  write.pBufferInfo = bufferInfos;
  write.pImageInfo = imageInfos;
  write.pTexelBufferView = nullptr;
  write.descriptorType = type;

  vkUpdateDescriptorSets(mDevice, 1, &write, 0, nullptr);
}

} // namespace quoll::rhi
