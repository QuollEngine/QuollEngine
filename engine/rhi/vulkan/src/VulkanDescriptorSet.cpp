#include "liquid/core/Base.h"

#include "VulkanDescriptorSet.h"
#include "VulkanMapping.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanTextureView.h"

namespace liquid::rhi {

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDeviceObject &device,
                                         const VulkanResourceRegistry &registry,
                                         VkDescriptorSet descriptorSet)
    : mDevice(device), mRegistry(registry), mDescriptorSet(descriptorSet) {}

void VulkanDescriptorSet::write(uint32_t binding,
                                const std::vector<TextureHandle> &textures,
                                DescriptorType type, uint32_t start) {
  std::vector<VkDescriptorImageInfo> imageInfos(textures.size(),
                                                VkDescriptorImageInfo{});

  for (size_t i = 0; i < textures.size(); ++i) {
    const auto &vkTexture = mRegistry.getTextures().at(textures.at(i));
    imageInfos.at(i).imageLayout =
        type == DescriptorType::StorageImage
            ? VK_IMAGE_LAYOUT_GENERAL
            : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfos.at(i).imageView = vkTexture->getImageView();
    imageInfos.at(i).sampler = vkTexture->getSampler();
  }

  write(binding, start, imageInfos.size(), type, imageInfos.data(), nullptr);
}

void VulkanDescriptorSet::write(
    uint32_t binding, const std::vector<TextureViewHandle> &textureViews,
    DescriptorType type, uint32_t start) {
  std::vector<VkDescriptorImageInfo> imageInfos(textureViews.size(),
                                                VkDescriptorImageInfo{});

  for (size_t i = 0; i < textureViews.size(); ++i) {
    const auto &vkTextureView =
        mRegistry.getTextureViews().at(textureViews.at(i));
    imageInfos.at(i).imageLayout =
        type == DescriptorType::StorageImage
            ? VK_IMAGE_LAYOUT_GENERAL
            : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfos.at(i).imageView = vkTextureView->getImageView();
    imageInfos.at(i).sampler = vkTextureView->getSampler();
  }

  write(binding, start, imageInfos.size(), type, imageInfos.data(), nullptr);
}

void VulkanDescriptorSet::write(uint32_t binding,
                                const std::vector<BufferHandle> &buffers,
                                DescriptorType type, uint32_t start) {
  std::vector<VkDescriptorBufferInfo> bufferInfos(buffers.size(),
                                                  VkDescriptorBufferInfo{});

  for (size_t i = 0; i < buffers.size(); ++i) {
    const auto &vkBuffer = mRegistry.getBuffers().at(buffers.at(i));
    bufferInfos.at(i).buffer = vkBuffer->getBuffer();
    bufferInfos.at(i).offset = 0;
    bufferInfos.at(i).range = vkBuffer->getSize();
  }

  write(binding, start, bufferInfos.size(), type, nullptr, bufferInfos.data());
}

void VulkanDescriptorSet::write(uint32_t binding, uint32_t start,
                                size_t descriptorCount, DescriptorType type,
                                const VkDescriptorImageInfo *imageInfos,
                                VkDescriptorBufferInfo *bufferInfos) {
  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.pNext = nullptr;
  write.dstBinding = binding;
  write.dstSet = mDescriptorSet;
  write.descriptorCount = static_cast<uint32_t>(descriptorCount);
  write.dstArrayElement = start;
  write.pBufferInfo = bufferInfos;
  write.pImageInfo = imageInfos;
  write.pTexelBufferView = nullptr;
  write.descriptorType = VulkanMapping::getDescriptorType(type);

  vkUpdateDescriptorSets(mDevice, 1, &write, 0, nullptr);
}

} // namespace liquid::rhi
