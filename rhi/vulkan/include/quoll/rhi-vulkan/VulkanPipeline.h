#pragma once

#include "quoll/rhi-vulkan/VulkanDeviceObject.h"
#include "quoll/rhi-vulkan/VulkanResourceRegistry.h"
#include "quoll/rhi-vulkan/VulkanPipelineLayoutCache.h"

#include "quoll/rhi/PipelineDescription.h"

#include "VulkanHeaders.h"

namespace quoll::rhi {

class VulkanPipeline {
public:
  VulkanPipeline(const GraphicsPipelineDescription &description,
                 VulkanDeviceObject &device,
                 const VulkanResourceRegistry &registry,
                 VulkanPipelineLayoutCache &pipelineLayoutCache);

  VulkanPipeline(const ComputePipelineDescription &description,
                 VulkanDeviceObject &device,
                 const VulkanResourceRegistry &registry,
                 VulkanPipelineLayoutCache &pipelineLayoutCache);

  ~VulkanPipeline();

  VulkanPipeline(const VulkanPipeline &) = delete;
  VulkanPipeline(VulkanPipeline &&) = delete;
  VulkanPipeline &operator=(const VulkanPipeline &) = delete;
  VulkanPipeline &operator=(VulkanPipeline &&) = delete;

  inline VkPipeline getPipeline() const { return mPipeline; }

  inline VkPipelineLayout getPipelineLayout() const { return mPipelineLayout; }

  inline const VkDescriptorSetLayout getDescriptorLayout(u32 index) const {
    return mDescriptorLayouts.at(index);
  }

  inline VkPipelineBindPoint getBindPoint() const { return mBindPoint; }

  const String &getDebugName() const { return mDebugName; }

private:
  void createLayout(const std::span<VulkanShader *> &shaders,
                    const VulkanResourceRegistry &registry,
                    VulkanPipelineLayoutCache &pipelineLayoutCache);

private:
  VulkanDeviceObject &mDevice;
  VkPipeline mPipeline = VK_NULL_HANDLE;
  VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
  VkPipelineBindPoint mBindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;

  std::unordered_map<u32, VkDescriptorSetLayout> mDescriptorLayouts;

  String mDebugName;
};

} // namespace quoll::rhi
