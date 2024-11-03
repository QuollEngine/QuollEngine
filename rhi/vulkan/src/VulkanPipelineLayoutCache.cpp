#include "quoll/core/Base.h"
#include "VulkanError.h"
#include "VulkanMapping.h"
#include "VulkanPipelineLayoutCache.h"

namespace quoll::rhi {

constexpr bool bindingsMatch(const VkDescriptorSetLayoutBinding &a,
                             const VkDescriptorSetLayoutBinding &b) {
  return (a.binding == b.binding) && (a.descriptorCount == b.descriptorCount) &&
         (a.descriptorType == b.descriptorType) &&
         (a.stageFlags == b.stageFlags);
}

constexpr bool bindingsMatch(const DescriptorLayoutBindingDescription &a,
                             const DescriptorLayoutBindingDescription &b) {
  return (a.binding == b.binding) && (a.name == b.name);
}

VulkanPipelineLayoutCache::VulkanPipelineLayoutCache(VulkanDeviceObject &device)
    : mDevice(device) {}

VulkanPipelineLayoutCache::~VulkanPipelineLayoutCache() {
  destroyAllDescriptorLayouts();
}

DescriptorLayoutHandle VulkanPipelineLayoutCache::getOrCreateDescriptorLayout(
    const DescriptorLayoutDescription &description) {
  for (usize i = 0; i < mDescriptorLayoutDescriptions.size(); ++i) {
    const auto &existing = mDescriptorLayoutDescriptions.at(i);
    if (existing.bindings.size() != description.bindings.size()) {
      continue;
    }

    bool matches = true;
    for (usize li = 0; li < description.bindings.size() && matches; ++li) {
      matches =
          bindingsMatch(description.bindings.at(li), existing.bindings.at(li));
    }

    if (matches) {
      return static_cast<DescriptorLayoutHandle>(i + 1);
    }
  }

  VkDescriptorSetLayout layout = createDescriptorLayout(description);
  mDescriptorLayoutDescriptions.push_back(description);
  mDescriptorSetLayouts.push_back(layout);

  return static_cast<DescriptorLayoutHandle>(mDescriptorSetLayouts.size());
}

void VulkanPipelineLayoutCache::clear() {
  destroyAllDescriptorLayouts();

  mDescriptorSetLayouts.clear();
  mDescriptorLayoutDescriptions.clear();
}

VkDescriptorSetLayout VulkanPipelineLayoutCache::createDescriptorLayout(
    const DescriptorLayoutDescription &description) {

  std::vector<VkDescriptorBindingFlags> vkBindingFlags(
      description.bindings.size(), VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
  std::vector<VkDescriptorSetLayoutBinding> vkBindings(
      description.bindings.size(), VkDescriptorSetLayoutBinding{});

  for (usize i = 0; i < description.bindings.size(); ++i) {
    auto &binding = description.bindings.at(i);

    if (binding.type == DescriptorLayoutBindingType::Dynamic) {
      vkBindingFlags.at(i) = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
                             VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
    }

    vkBindings.at(i).binding = binding.binding;
    vkBindings.at(i).descriptorCount = binding.descriptorCount;
    vkBindings.at(i).descriptorType =
        VulkanMapping::getDescriptorType(binding.descriptorType);
    vkBindings.at(i).pImmutableSamplers = nullptr;
    vkBindings.at(i).stageFlags =
        VulkanMapping::getShaderStageFlags(binding.shaderStage);
  }

  VkDescriptorSetLayoutBindingFlagsCreateInfoEXT bindingFlagsCreateInfo{};
  bindingFlagsCreateInfo.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
  bindingFlagsCreateInfo.pNext = nullptr;
  bindingFlagsCreateInfo.pBindingFlags = vkBindingFlags.data();
  bindingFlagsCreateInfo.bindingCount = static_cast<u32>(vkBindingFlags.size());

  VkDescriptorSetLayout layout = VK_NULL_HANDLE;
  VkDescriptorSetLayoutCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  createInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
  createInfo.pNext = &bindingFlagsCreateInfo;
  createInfo.bindingCount = static_cast<u32>(vkBindings.size());
  createInfo.pBindings = vkBindings.data();

  checkForVulkanError(
      vkCreateDescriptorSetLayout(mDevice, &createInfo, nullptr, &layout),
      "Failed to create descriptor set layout", description.debugName);

  mDevice.setObjectName(description.debugName,
                        VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, layout);

  return layout;
}

void VulkanPipelineLayoutCache::destroyAllDescriptorLayouts() {
  for (VkDescriptorSetLayout layout : mDescriptorSetLayouts) {
    vkDestroyDescriptorSetLayout(mDevice, layout, nullptr);
  }
}

} // namespace quoll::rhi
