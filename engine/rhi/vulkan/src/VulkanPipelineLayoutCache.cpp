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
    : mDevice(device) {
  createGlobalTexturesDescriptorLayout();
}

VkDescriptorSetLayout VulkanPipelineLayoutCache::getOrCreateDescriptorLayout(
    const VulkanShader::ReflectionDescriptorSetLayout &info) {

  // Bindless textures
  if (info.names.size() == 1 && info.names.at(0) == "uGlobalTextures") {
    return mDescriptorSetLayouts.at(0);
  }

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

  VkDescriptorSetLayout layout =
      createDescriptorLayout(info, VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
  mDescriptorSetLayoutData.push_back(info);
  mDescriptorSetLayouts.push_back(layout);

  return layout;
}

VulkanPipelineLayoutCache::~VulkanPipelineLayoutCache() {
  destroyAllDescriptorLayouts();
}

void VulkanPipelineLayoutCache::clear() {
  destroyAllDescriptorLayouts();

  mDescriptorSetLayouts.clear();
  mDescriptorSetLayoutData.clear();

  createGlobalTexturesDescriptorLayout();
}

VkDescriptorSetLayout VulkanPipelineLayoutCache::createDescriptorLayout(
    const VulkanShader::ReflectionDescriptorSetLayout &info,
    VkDescriptorBindingFlags flags) {
  std::vector<VkDescriptorBindingFlags> bindingFlags(info.bindings.size(),
                                                     flags);

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
  createInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
  createInfo.pNext = &bindingFlagsCreateInfo;
  createInfo.bindingCount = static_cast<uint32_t>(info.bindings.size());
  createInfo.pBindings = info.bindings.data();

  checkForVulkanError(
      vkCreateDescriptorSetLayout(mDevice, &createInfo, nullptr, &layout),
      "Failed to create descriptor set layout");

  return layout;
}

void VulkanPipelineLayoutCache::createGlobalTexturesDescriptorLayout() {
  static constexpr uint32_t NumSamplers = 1000;

  VulkanShader::ReflectionDescriptorSetLayout info{};

  info.names.push_back("uGlobalTextures");

  VkDescriptorSetLayoutBinding binding{};
  binding.pImmutableSamplers = nullptr;
  binding.binding = 0;
  binding.descriptorCount = NumSamplers;
  binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  info.bindings.push_back(binding);

  VkDescriptorSetLayout layout = createDescriptorLayout(
      info, VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
                VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT |
                VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT);

  mDescriptorSetLayoutData.push_back(info);
  mDescriptorSetLayouts.push_back(layout);
}

void VulkanPipelineLayoutCache::destroyAllDescriptorLayouts() {
  for (VkDescriptorSetLayout layout : mDescriptorSetLayouts) {
    vkDestroyDescriptorSetLayout(mDevice, layout, nullptr);
  }
}

} // namespace liquid::rhi
