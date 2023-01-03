#include "liquid/core/Base.h"

#include <vulkan/vulkan.hpp>

#include "VulkanPipelineLayoutCache.h"
#include "VulkanError.h"

namespace liquid::rhi {

/**
 * @brief Check if two descriptor set bindings match
 *
 * @param a First binding
 * @param b Second binding
 * @retval true Bindings match
 * @retval false Bindings do not match
 */
static bool bindingsMatch(const VkDescriptorSetLayoutBinding &a,
                          const VkDescriptorSetLayoutBinding &b) {
  return (a.binding == b.binding) && (a.descriptorCount == b.descriptorCount) &&
         (a.descriptorType == b.descriptorType) &&
         (a.stageFlags == b.stageFlags);
}

VulkanPipelineLayoutCache::VulkanPipelineLayoutCache(VulkanDeviceObject &device)
    : mDevice(device) {}

VkDescriptorSetLayout VulkanPipelineLayoutCache::getOrCreateDescriptorLayout(
    const VulkanShader::ReflectionDescriptorSetLayout &info) {

  for (size_t i = 0; i < mDescriptorSetLayoutData.size(); ++i) {
    const auto &existingInfo = mDescriptorSetLayoutData.at(i);
    if (existingInfo.bindings.size() != info.bindings.size()) {
      continue;
    }

    bool matches = true;
    for (size_t li = 0; li < info.bindings.size() && matches; ++li) {
      matches =
          bindingsMatch(info.bindings.at(li), existingInfo.bindings.at(li));
    }

    if (matches) {
      return mDescriptorSetLayouts.at(i);
    }
  }

  VkDescriptorSetLayout layout = createDescriptorLayout(info);
  mDescriptorSetLayoutData.push_back(info);
  mDescriptorSetLayouts.push_back(layout);

  return layout;
}

VulkanPipelineLayoutCache::~VulkanPipelineLayoutCache() { clear(); }

void VulkanPipelineLayoutCache::clear() {
  for (VkDescriptorSetLayout layout : mDescriptorSetLayouts) {
    vkDestroyDescriptorSetLayout(mDevice, layout, nullptr);
  }

  mDescriptorSetLayouts.clear();
  mDescriptorSetLayoutData.clear();
}

VkDescriptorSetLayout VulkanPipelineLayoutCache::createDescriptorLayout(
    const VulkanShader::ReflectionDescriptorSetLayout &info) {
  std::vector<VkDescriptorBindingFlags> bindingFlags(
      info.bindings.size(), VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);

  VkDescriptorSetLayoutBindingFlagsCreateInfoEXT bindingFlagsCreateInfo{};
  bindingFlagsCreateInfo.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
  bindingFlagsCreateInfo.pNext = nullptr;
  bindingFlagsCreateInfo.pBindingFlags = bindingFlags.data();
  bindingFlagsCreateInfo.bindingCount =
      static_cast<uint32_t>(bindingFlags.size());

  VkDescriptorSetLayout layout = VK_NULL_HANDLE;
  VkDescriptorSetLayoutCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  createInfo.flags = 0;
  createInfo.pNext = &bindingFlagsCreateInfo;
  createInfo.bindingCount = static_cast<uint32_t>(info.bindings.size());
  createInfo.pBindings = info.bindings.data();

  checkForVulkanError(
      vkCreateDescriptorSetLayout(mDevice, &createInfo, nullptr, &layout),
      "Failed to create descriptor set layout");

  return layout;
}

} // namespace liquid::rhi
